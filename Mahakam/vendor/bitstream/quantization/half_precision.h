#pragma once

/*
 *  Copyright (c) 2018 Stanislav Denisov
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include <cstdint>
#include <cstring>

namespace bitstream
{
	/**
	 * @brief Class for quantizing single-precision floats into half-precision
	*/
	class half_precision
	{
	public:
		inline static uint16_t quantize(float value) noexcept
		{
			int32_t tmp;
			std::memcpy(&tmp, &value, sizeof(float));

			int32_t s = (tmp >> 16) & 0x00008000;
			int32_t e = ((tmp >> 23) & 0X000000FF) - (127 - 15);
			int32_t m = tmp & 0X007FFFFF;

			if (e <= 0) {
				if (e < -10)
					return static_cast<uint16_t>(s);

				m |= 0x00800000;

				int32_t t = 14 - e;
				int32_t a = (1 << (t - 1)) - 1;
				int32_t b = (m >> t) & 1;

				m = (m + a + b) >> t;

				return static_cast<uint16_t>(s | m);
			}

			if (e == 0XFF - (127 - 15)) {
				if (m == 0)
					return static_cast<uint16_t>(s | 0X7C00);

				m >>= 13;

				return static_cast<uint16_t>(s | 0X7C00 | m | ((m == 0) ? 1 : 0));
			}

			m = m + 0X00000FFF + ((m >> 13) & 1);

			if ((m & 0x00800000) != 0) {
				m = 0;
				e++;
			}

			if (e > 30)
				return static_cast<uint16_t>(s | 0X7C00);

			return static_cast<uint16_t>(s | (e << 10) | (m >> 13));
		}

		inline static float dequantize(uint16_t value) noexcept
		{
			uint32_t tmp;
			uint32_t mantissa = static_cast<uint32_t>(value & 1023);
			uint32_t exponent = 0XFFFFFFF2;

			if ((value & -33792) == 0) {
				if (mantissa != 0) {
					while ((mantissa & 1024) == 0) {
						exponent--;
						mantissa <<= 1;
					}

					mantissa &= 0XFFFFFBFF;
					tmp = ((static_cast<uint32_t>(value) & 0x8000) << 16) | ((exponent + 127) << 23) | (mantissa << 13);
				}
				else
				{
					tmp = static_cast<uint32_t>((value & 0x8000) << 16);
				}
			}
			else
			{
				tmp = ((static_cast<uint32_t>(value) & 0x8000) << 16) | (((((static_cast<uint32_t>(value) >> 10) & 0X1F) - 15) + 127) << 23) | (mantissa << 13);
			}

			float result;
			std::memcpy(&result, &tmp, sizeof(float));

			return result;
		}
	};
}