#pragma once

#include "Core.h"

namespace Mahakam
{
	class Packer
	{
	public:
		struct Block;
		struct Node
		{
			float x;
			float y;
			float w;
			float h;
			bool used = false;
			Node* down;
			Node* right;
		};

		struct Block
		{
			float w;
			float h;
			float x = 0.0f;
			float y = 0.0f;
		};

		Node* root;

		std::vector<Node*> nodes;

		Packer(Block* blocks, uint32_t amount)
		{
			root = new Node{ 0.0f, 0.0f, 8192.0f, 8192.0f };
			nodes.push_back(root);

			for (int n = 0; n < amount; n++)
			{
				Block& block = blocks[n];
				Node* node = findNode(root, block.w, block.h);
				if (node)
				{
					splitNode(node, block.w, block.h);
					nodes.push_back(node->down);
					nodes.push_back(node->right);
					block.x = node->x;
					block.y = node->y;
				}
			}

			for (auto& node : nodes)
				delete node;
		}

		Node* findNode(Node* parent, float w, float h)
		{
			if (parent->used)
			{
				Packer::Node* childRight = findNode(parent->right, w, h);
				if (childRight)
					return childRight;

				return findNode(parent->down, w, h);
			}
			else if ((w <= w) && (h <= h))
				return parent;
			else
				return nullptr;
		}

		void splitNode(Node* parent, float w, float h)
		{
			parent->used = true;
			parent->down = new Packer::Node{ parent->x, parent->y + h, w, h - h };
			parent->right = new Packer::Node{ parent->x + w, parent->y, w - w, h };
		}
	};
}