#include "mhpch.h"
#define MINIAUDIO_IMPLEMENTATION
#include "AudioBuild.h"

MA_API ma_result ma_result_from_IPLerror(IPLerror error)
{
	switch (error)
	{
	case IPL_STATUS_SUCCESS:     return MA_SUCCESS;
	case IPL_STATUS_OUTOFMEMORY: return MA_OUT_OF_MEMORY;
	case IPL_STATUS_INITIALIZATION:
	case IPL_STATUS_FAILURE:
	default: return MA_ERROR;
	}
}


MA_API ma_steamaudio_binaural_node_config ma_steamaudio_binaural_node_config_init(ma_uint32 channelsIn, IPLAudioSettings iplAudioSettings, IPLContext iplContext, IPLHRTF iplHRTF)
{
	ma_steamaudio_binaural_node_config config;

	MA_ZERO_OBJECT(&config);
	config.nodeConfig = ma_node_config_init();
	config.channelsIn = channelsIn;
	config.iplAudioSettings = iplAudioSettings;
	config.iplContext = iplContext;
	config.iplHRTF = iplHRTF;

	return config;
}


static void ma_steamaudio_binaural_node_process_pcm_frames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut)
{
	ma_steamaudio_binaural_node* pBinauralNode = (ma_steamaudio_binaural_node*)pNode;
	IPLBinauralEffectParams binauralParams;
	IPLDirectEffectParams directParams;
	IPLAudioBuffer inputBufferDesc;
	IPLAudioBuffer middleBufferDesc;
	IPLAudioBuffer outputBufferDesc;
	ma_uint32 totalFramesToProcess = *pFrameCountOut;
	ma_uint32 totalFramesProcessed = 0;

	// Binaural
	binauralParams.direction.x = pBinauralNode->direction.x;
	binauralParams.direction.y = pBinauralNode->direction.y;
	binauralParams.direction.z = pBinauralNode->direction.z;
	binauralParams.interpolation = pBinauralNode->interpolate ? IPL_HRTFINTERPOLATION_BILINEAR : IPL_HRTFINTERPOLATION_NEAREST;
	binauralParams.spatialBlend = pBinauralNode->spatialBlend;
	binauralParams.hrtf = pBinauralNode->iplHRTF;

	float a = 1.0f;
	float b = pBinauralNode->distance;
	float t = pBinauralNode->spatialBlend;
	float distanceLerp = a + t * (b - a);

	// Direct
	directParams.flags = (IPLDirectEffectFlags)(IPL_DIRECTEFFECTFLAGS_APPLYDISTANCEATTENUATION | IPL_DIRECTEFFECTFLAGS_APPLYAIRABSORPTION);
	directParams.distanceAttenuation = distanceLerp;
	directParams.airAbsorption[0] = pBinauralNode->airAbsorption[0];
	directParams.airAbsorption[1] = pBinauralNode->airAbsorption[1];
	directParams.airAbsorption[2] = pBinauralNode->airAbsorption[2];


	inputBufferDesc.numChannels = (IPLint32)ma_node_get_input_channels(pNode, 0);

	middleBufferDesc.numChannels = 1;

	/* We'll run this in a loop just in case our deinterleaved buffers are too small. */
	outputBufferDesc.numSamples = pBinauralNode->iplAudioSettings.frameSize;
	outputBufferDesc.numChannels = 2;
	outputBufferDesc.data = pBinauralNode->ppBuffersOut;

	while (totalFramesProcessed < totalFramesToProcess) {
		ma_uint32 framesToProcessThisIteration = totalFramesToProcess - totalFramesProcessed;
		if (framesToProcessThisIteration > (ma_uint32)pBinauralNode->iplAudioSettings.frameSize)
			framesToProcessThisIteration = (ma_uint32)pBinauralNode->iplAudioSettings.frameSize;

		if (pBinauralNode->spatialBlend <= 0.0f)
		{
			ma_copy_pcm_frames(ma_offset_pcm_frames_ptr_f32(ppFramesOut[0], totalFramesProcessed, 2), ma_offset_pcm_frames_const_ptr_f32(ppFramesIn[0], totalFramesProcessed, 2), framesToProcessThisIteration, ma_format_f32, inputBufferDesc.numChannels);
			totalFramesProcessed += framesToProcessThisIteration;
			continue;
		}

		if (inputBufferDesc.numChannels == 1)
		{
			/* Fast path. No need for deinterleaving since it's a mono stream. */
			pBinauralNode->ppBuffersIn[0] = (float*)ma_offset_pcm_frames_const_ptr_f32(ppFramesIn[0], totalFramesProcessed, 1);
		}
		else
		{
			/* Slow path. Need to deinterleave the input data. */
			ma_deinterleave_pcm_frames(ma_format_f32, inputBufferDesc.numChannels, framesToProcessThisIteration, ma_offset_pcm_frames_const_ptr_f32(ppFramesIn[0], totalFramesProcessed, inputBufferDesc.numChannels), (void**)pBinauralNode->ppBuffersIn);
		}

		inputBufferDesc.data = pBinauralNode->ppBuffersIn;
		inputBufferDesc.numSamples = (IPLint32)framesToProcessThisIteration;

		middleBufferDesc.data = pBinauralNode->ppBuffersIn;
		middleBufferDesc.numSamples = (IPLint32)framesToProcessThisIteration;

		/* Apply the direct effect. */
		iplDirectEffectApply(pBinauralNode->iplDirectEffect, &directParams, &inputBufferDesc, &middleBufferDesc);

		/* Apply the binaural effect. */
		iplBinauralEffectApply(pBinauralNode->iplBinauralEffect, &binauralParams, &middleBufferDesc, &outputBufferDesc);

		/* Interleave straight into the output buffer. */
		ma_interleave_pcm_frames(ma_format_f32, 2, framesToProcessThisIteration, (const void**)pBinauralNode->ppBuffersOut, ma_offset_pcm_frames_ptr_f32(ppFramesOut[0], totalFramesProcessed, 2));

		/* Advance. */
		totalFramesProcessed += framesToProcessThisIteration;
	}

	(void)pFrameCountIn;    /* Unused. */
}

static ma_node_vtable g_ma_steamaudio_binaural_node_vtable =
{
	ma_steamaudio_binaural_node_process_pcm_frames,
	NULL,
	1,  /* 1 input channel. */
	1,  /* 1 output channel. */
	0
};

MA_API ma_result ma_steamaudio_binaural_node_init(ma_node_graph* pNodeGraph, const ma_steamaudio_binaural_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_steamaudio_binaural_node* pBinauralNode)
{
	ma_result result;
	ma_node_config baseConfig;
	ma_uint32 channelsIn;
	ma_uint32 channelsOut;
	IPLBinauralEffectSettings iplBinauralEffectSettings;
	IPLDirectEffectSettings iplDirectEffecSettings;
	size_t heapSizeInBytes;

	if (pBinauralNode == NULL)
		return MA_INVALID_ARGS;

	MA_ZERO_OBJECT(pBinauralNode);

	if (pConfig == NULL || pConfig->iplAudioSettings.frameSize == 0 || pConfig->iplContext == NULL || pConfig->iplHRTF == NULL)
		return MA_INVALID_ARGS;

	/* Steam Audio only supports mono and stereo input. */
	if (pConfig->channelsIn < 1 || pConfig->channelsIn > 2)
		return MA_INVALID_ARGS;

	channelsIn = pConfig->channelsIn;
	channelsOut = 2;    /* Always stereo output. */

	baseConfig = ma_node_config_init();
	baseConfig.vtable = &g_ma_steamaudio_binaural_node_vtable;
	baseConfig.pInputChannels = &channelsIn;
	baseConfig.pOutputChannels = &channelsOut;
	result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pBinauralNode->baseNode);
	if (result != MA_SUCCESS)
	{
		return result;
	}

	pBinauralNode->iplAudioSettings = pConfig->iplAudioSettings;
	pBinauralNode->iplContext = pConfig->iplContext;
	pBinauralNode->iplHRTF = pConfig->iplHRTF;

	MA_ZERO_OBJECT(&iplBinauralEffectSettings);
	iplBinauralEffectSettings.hrtf = pBinauralNode->iplHRTF;

	result = ma_result_from_IPLerror(iplBinauralEffectCreate(pBinauralNode->iplContext, &pBinauralNode->iplAudioSettings, &iplBinauralEffectSettings, &pBinauralNode->iplBinauralEffect));
	if (result != MA_SUCCESS)
	{
		ma_node_uninit(&pBinauralNode->baseNode, pAllocationCallbacks);
		return result;
	}

	MA_ZERO_OBJECT(&iplDirectEffecSettings);
	iplDirectEffecSettings.numChannels = 1;

	result = ma_result_from_IPLerror(iplDirectEffectCreate(pBinauralNode->iplContext, &pBinauralNode->iplAudioSettings, &iplDirectEffecSettings, &pBinauralNode->iplDirectEffect));
	if (result != MA_SUCCESS)
	{
		iplBinauralEffectRelease(&pBinauralNode->iplBinauralEffect);
		ma_node_uninit(&pBinauralNode->baseNode, pAllocationCallbacks);
		return result;
	}

	heapSizeInBytes = 0;

	/*
	Unfortunately Steam Audio uses deinterleaved buffers for everything so we'll need to use some
	intermediary buffers. We'll allocate one big buffer on the heap and then use offsets. We'll
	use the frame size from the IPLAudioSettings structure as a basis for the size of the buffer.
	*/
	heapSizeInBytes += sizeof(float) * channelsOut * pBinauralNode->iplAudioSettings.frameSize; /* Output buffer. */
	heapSizeInBytes += sizeof(float) * channelsIn * pBinauralNode->iplAudioSettings.frameSize; /* Input buffer. */

	pBinauralNode->_pHeap = ma_malloc(heapSizeInBytes, pAllocationCallbacks);
	if (pBinauralNode->_pHeap == NULL)
	{
		iplBinauralEffectRelease(&pBinauralNode->iplBinauralEffect);
		iplDirectEffectRelease(&pBinauralNode->iplDirectEffect);
		ma_node_uninit(&pBinauralNode->baseNode, pAllocationCallbacks);
		return MA_OUT_OF_MEMORY;
	}

	pBinauralNode->ppBuffersOut[0] = (float*)pBinauralNode->_pHeap;
	pBinauralNode->ppBuffersOut[1] = (float*)ma_offset_ptr(pBinauralNode->_pHeap, sizeof(float) * pBinauralNode->iplAudioSettings.frameSize);

	{
		ma_uint32 iChannelIn;
		for (iChannelIn = 0; iChannelIn < channelsIn; iChannelIn += 1) {
			pBinauralNode->ppBuffersIn[iChannelIn] = (float*)ma_offset_ptr(pBinauralNode->_pHeap, sizeof(float) * pBinauralNode->iplAudioSettings.frameSize * (channelsOut + iChannelIn));
		}
	}

	return MA_SUCCESS;
}

MA_API void ma_steamaudio_binaural_node_uninit(ma_steamaudio_binaural_node* pBinauralNode, const ma_allocation_callbacks* pAllocationCallbacks)
{
	if (pBinauralNode == NULL)
		return;

	/* The base node is always uninitialized first. */
	ma_node_uninit(&pBinauralNode->baseNode, pAllocationCallbacks);

	/*
	The Steam Audio objects are deleted after the base node. This ensures the base node is removed from the graph
	first to ensure these objects aren't getting used by the audio thread.
	*/
	iplBinauralEffectRelease(&pBinauralNode->iplBinauralEffect);
	iplDirectEffectRelease(&pBinauralNode->iplDirectEffect);
	ma_free(pBinauralNode->_pHeap, pAllocationCallbacks);
}

MA_API ma_result ma_steamaudio_binaural_node_set_position(ma_steamaudio_binaural_node* pBinauralNode, IPLVector3 direction, IPLVector3 source, IPLVector3 listener)
{
	if (pBinauralNode == NULL)
		return MA_INVALID_ARGS;

	// Distance modelling
	IPLDistanceAttenuationModel distanceAttenuationModel{};
	distanceAttenuationModel.type = IPL_DISTANCEATTENUATIONTYPE_DEFAULT;

	float distanceAttenuation = iplDistanceAttenuationCalculate(pBinauralNode->iplContext, source, listener, &distanceAttenuationModel);

	// Air absorption modelling
	IPLAirAbsorptionModel airAbsorptionModel{};
	airAbsorptionModel.type = IPL_AIRABSORPTIONTYPE_DEFAULT;

	iplAirAbsorptionCalculate(pBinauralNode->iplContext, source, listener, &airAbsorptionModel, pBinauralNode->airAbsorption);

	pBinauralNode->direction = direction;

	pBinauralNode->distance = distanceAttenuation;

	return MA_SUCCESS;
}