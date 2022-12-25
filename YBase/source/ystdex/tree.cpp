/*
	© 2018, 2020-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file tree.cpp
\ingroup YStandardEx
\brief 作为关联容器实现的树。
\version r416
\author FrankHB <frankhb1989@gmail.com>
\since build 830
\par 创建时间:
	2018-07-06 21:15:48 +0800
\par 修改时间:
	2022-11-28 19:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Tree
*/


#include "ystdex/tree.h"

namespace ystdex
{

namespace details
{

inline namespace rb_tree
{

//! \since build 830
namespace
{

YB_PURE tree_node_base*
local_tree_increment(tree_node_base* x) ynothrow
{
	if(x->right)
	{
		x = x->right;
		while(x->left)
			x = x->left;
	}
	else
	{
		auto y(x->parent);

		while(x == y->right)
		{
			x = y;
			y = y->parent;
		}
		if(x->right != y)
			x = y;
	}
	return x;
}

YB_PURE tree_node_base*
local_tree_decrement(tree_node_base* x) ynothrow
{
	if(x->color == tree_color::red && x->parent->parent == x)
		x = x->right;
	else if(x->left)
	{
		auto y(x->left);

		while(y->right)
			y = y->right;
		x = y;
	}
	else
	{
		auto y(x->parent);

		while(x == y->left)
		{
			x = y;
			y = y->parent;
		}
		x = y;
	}
	return x;
}

void
local_tree_rotate_left(tree_node_base* x, tree_node_base*& root)
{
	const auto y = x->right;

	x->right = y->left;
	if(y->left)
		y->left->parent = x;
	y->parent = x->parent;
	if(x == root)
		root = y;
	else if(x == x->parent->left)
		x->parent->left = y;
	else
		x->parent->right = y;
	y->left = x;
	x->parent = y;
}

void
local_tree_rotate_right(tree_node_base* x, tree_node_base*& root)
{
	const auto y = x->left;

	x->left = y->right;
	if(y->right)
		y->right->parent = x;
	y->parent = x->parent;
	if(x == root)
		root = y;
	else if(x == x->parent->right)
		x->parent->right = y;
	else
		x->parent->left = y;
	y->right = x;
	x->parent = y;
}

} // unnamed namespace;


tree_node_base*
tree_increment(tree_node_base* x) ynothrow
{
	return local_tree_increment(x);
}
const tree_node_base*
tree_increment(const tree_node_base* x) ynothrow
{
	return local_tree_increment(const_cast<tree_node_base*>(x));
}

tree_node_base*
tree_decrement(tree_node_base* x) ynothrow
{
	return local_tree_decrement(x);
}
const tree_node_base*
tree_decrement(const tree_node_base* x) ynothrow
{
	return local_tree_decrement(const_cast<tree_node_base*>(x));
}

void
tree_insert_and_rebalance(bool insert_left, tree_node_base* x,
	tree_node_base* p, tree_node_base& header) ynothrow
{
	auto& root(header.parent);

	// XXX: Initialized here for new node.
	x->parent = p;
	x->left = {};
	x->right = {};
	x->color = tree_color::red;

	// NOTE: Make new node child of parent and maintain root, leftmost and
	// rightmost nodes. First node is always inserted left.
	if(insert_left)
	{
		// NOTE: Also makes leftmost = x when 'p == &header'.
		p->left = x;
		if(p == &header)
		{
			header.parent = x;
			header.right = x;
		}
		else if(p == header.left)
			header.left = x;
	}
	else
	{
		p->right = x;
		if(p == header.right)
			header.right = x;
	}
	// NOTE: Rebalance.
	while(x != root && x->parent->color == tree_color::red)
	{
		const auto xpp(x->parent->parent);

		if(x->parent == xpp->left)
		{
			const auto y(xpp->right);

			if(y && y->color == tree_color::red)
			{
				x->parent->color = tree_color::black;
				y->color = tree_color::black;
				xpp->color = tree_color::red;
				x = xpp;
			}
			else
			{
				if(x == x->parent->right)
				{
					x = x->parent;
					local_tree_rotate_left(x, root);
				}
				x->parent->color = tree_color::black;
				xpp->color = tree_color::red;
				local_tree_rotate_right(xpp, root);
			}
		}
		else
		{
			const auto y(xpp->left);

			if(y && y->color == tree_color::red)
			{
				x->parent->color = tree_color::black;
				y->color = tree_color::black;
				xpp->color = tree_color::red;
				x = xpp;
			}
			else
			{
				if(x == x->parent->left)
				{
					x = x->parent;
					local_tree_rotate_right(x, root);
				}
				x->parent->color = tree_color::black;
				xpp->color = tree_color::red;
				local_tree_rotate_left(xpp, root);
			}
		}
	}
	root->color = tree_color::black;
}

tree_node_base*
tree_rebalance_for_erase(tree_node_base* z, tree_node_base& header) ynothrow
{
	auto& root(header.parent);
	auto& leftmost(header.left);
	auto& rightmost(header.right);
	auto y(z);
	tree_node_base* x{};
	tree_node_base* x_parent{};

	if(!y->left)
		x = y->right;
	else if(!y->right)
		x = y->left;
	else
	{
		y = y->right;
		while(y->left)
			y = y->left;
		x = y->right;
	}
	if(y != z)
	{
		z->left->parent = y;
		y->left = z->left;
		if(y != z->right)
		{
			x_parent = y->parent;
			if(x)
				x->parent = y->parent;
			y->parent->left = x;
			y->right = z->right;
			z->right->parent = y;
		}
		else
			x_parent = y;
		if(root == z)
			root = y;
		else if(z->parent->left == z)
			z->parent->left = y;
		else
			z->parent->right = y;
		y->parent = z->parent;
		std::swap(y->color, z->color);
		y = z;
	}
	else
	{
		x_parent = y->parent;
		if(x)
			x->parent = y->parent;
		if(root == z)
			root = x;
		else if(z->parent->left == z)
			z->parent->left = x;
		else
			z->parent->right = x;
		if(leftmost == z)
			leftmost = !z->right ? z->parent : tree_node_base::minimum(x);
		if(rightmost == z)
			rightmost = !z->left ? z->parent : tree_node_base::maximum(x);
	}
	if(y->color != tree_color::red)
	{
		while(x != root && (!x || x->color == tree_color::black))
			if(x == x_parent->left)
			{
				auto w(x_parent->right);

				if(w->color == tree_color::red)
				{
					w->color = tree_color::black;
					x_parent->color = tree_color::red;
					local_tree_rotate_left(x_parent, root);
					w = x_parent->right;
				}
				if((!w->left || w->left->color == tree_color::black)
					&& (!w->right || w->right->color == tree_color::black))
				{
					w->color = tree_color::red;
					x = x_parent;
					x_parent = x_parent->parent;
				}
				else
				{
					if(!w->right || w->right->color == tree_color::black)
					{
						w->left->color = tree_color::black;
						w->color = tree_color::red;
						local_tree_rotate_right(w, root);
						w = x_parent->right;
					}
					w->color = x_parent->color;
					x_parent->color = tree_color::black;
					if(w->right)
						w->right->color = tree_color::black;
					local_tree_rotate_left(x_parent, root);
					break;
				}
			}
			else
			{
				auto w(x_parent->left);

				if(w->color == tree_color::red)
				{
					w->color = tree_color::black;
					x_parent->color = tree_color::red;
					local_tree_rotate_right(x_parent, root);
					w = x_parent->left;
				}
				if((!w->right || w->right->color == tree_color::black)
					&& (!w->left
						   || w->left->color == tree_color::black))
				{
					w->color = tree_color::red;
					x = x_parent;
					x_parent = x_parent->parent;
				}
				else
				{
					if(!w->left || w->left->color == tree_color::black)
					{
						w->right->color = tree_color::black;
						w->color = tree_color::red;
						local_tree_rotate_left(w, root);
						w = x_parent->left;
					}
					w->color = x_parent->color;
					x_parent->color = tree_color::black;
					if(w->left)
						w->left->color = tree_color::black;
					local_tree_rotate_right(x_parent, root);
					break;
				}
			}
		if(x)
			x->color = tree_color::black;
	}
	return y;
}

size_t
tree_black_count(const tree_node_base* node, const tree_node_base* root)
	ynothrow
{
	if(node)
	{
		size_t sum(0);

		while([&]{
			if(node->color == tree_color::black)
				++sum;
			return node != root;
		}())
			node = node->parent;
		return sum;
	}
	return 0;
}

} // inline namespace rb_tree;

} // namespace details;

} // namespace ystdex;

