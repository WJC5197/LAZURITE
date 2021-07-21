#pragma once
#ifndef _TREE_H_
#define _TREE_H_
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <tuple>
#include <utility>
#include <cassert>
#include <memory.h>
#include <cstdio>
#include <cstdlib>
#include <stdarg.h>
#include <stdarg.h>
#include <windows.h>
#include "graphics.h"

using namespace std;
//1gb=2^(30)个double数据类型
//改进：1.末尾指针指向层首指针
template <typename T> class tree
		  //three ways to construct:
		  //1.defalut,without head value.
		  //2.head value.
		  //3.initialize the nodes with maximum  number of has_nodes children.
{
public:
	struct node//树的节点
	{
		friend class tree;

		template <typename T>
		friend class binary_tree;

		template <typename T>
		friend binary_tree<T> add_tree(const binary_tree<T>&tree_added,const binary_tree<T>&tree_toadd);

	public:
		__int64 has_nodes = 0;
		__int64 self_layer = 1;
		T value;
		node()
		{

		}
		node(__int64 has_nodes,T value)
		{
			this->has_nodes = has_nodes;
			this->value = value;
		}
		node(__int64 has_nodes,__int64 self_layer,T value)
		{
			this->has_nodes = has_nodes;
			this->self_layer = self_layer;
			this->value = value;
		}
	protected:
		node* sibling = nullptr;
		vector<node*>ptr;
	};
	__int64 nodes_sum;
	__int64 layer_num = 1;	
	__int64 sum_nodes(unsigned __int64 layer_num)
	{
		auto nodes_sum_lambda = [this](__int64 layer_num) ->__int64 {__int64 sum = 0; for (__int64 i = 0; i < layer_num; i++)sum += get<0>(layer[i]); return sum; };
		return nodes_sum_lambda(layer_num);
	}

	tree()
	{
		this->head_ptr = new node;//头指针
		this->ope_ptr = this->head_ptr;//最近操作的指针
		this->parent_ptr = this->head_ptr;//因为只有一个节点，所以先暂时把双亲节点设置为本节点
		this->head_ptr->has_nodes = 0;
		this->head_ptr->self_layer = 1;
		this->layer.insert(pair<__int64, tuple<__int64, node*>>(1, { 1, head_ptr }));//第一层层参数
		this->nodes_sum = 1;//算出节点总数
	}

	tree(T value)
	{
		this->head_ptr = new node;//头指针
		this->ope_ptr = this->head_ptr;//最近操作的指针
		this->parent_ptr = this->head_ptr;//因为只有一个节点，所以先暂时把双亲节点设置为本节点
		this->head_ptr->value = value;//头指针值
		this->head_ptr->has_nodes = 0;
		this->head_ptr->self_layer = 1;
		this->layer.insert(pair<__int64, tuple<__int64, node*>>(1, { 1, head_ptr }));//第一层层参数
		this->nodes_sum = 1;//算出节点总数
	}

	tree(T* list_node,__int64 has_nodes,__int64 n)//use an array to construct a n-way tree.
	{
		this->head_ptr = new node;//头指针
		this->ope_ptr = this->head_ptr;//最近操作的指针
		this->parent_ptr = this->head_ptr;//因为只有一个节点，所以先暂时把双亲节点设置为本节点
		this->head_ptr->value = list_node[0];//头指针值
		this->head_ptr->has_nodes = 0;
		this->head_ptr->self_layer = 1;
		this->layer.insert(pair<__int64, tuple<__int64, node*>>(1, { 1, head_ptr }));//第一层层参数
		this->nodes_sum = 1;//算出节点总数
		node* save_ptr=nullptr;
		this->add_node(list_node[1], 1);
		for (__int64 i = 2; i < n; i++)
		{
			if (this->parent_ptr->has_nodes < has_nodes)
			{
				this->add_node(list_node[i], 0);
			}
			else
			{
				tuple<__int64, __int64>temp = this->return_ptr(this->parent_ptr);
				this->change_ope(get<0>(temp), get<1>(temp));
				if (this->ope_ptr->sibling == nullptr)
				{
					tuple<__int64, __int64>temp_next = this->return_ptr(get<1>(this->layer[this->ope_ptr->self_layer+1]));
					this->change_ope(get<0>(temp_next),get<1>(temp_next));
					this->add_node(list_node[i], 1);
				}
				else
				{
					temp = this->return_ptr(this->ope_ptr->sibling);
					this->change_ope(get<0>(temp), get<1>(temp));
					this->add_node(list_node[i], 1);
				}
			}
		}
	}
	//析构linked_list
	~tree()
	{
		
	}

	tree& operator=(tree& other)//problem
	{
		delete this->head_ptr;
		this->layer.clear();
		this->head_ptr = new node(other.head_ptr->has_nodes,other.head_ptr->self_layer,other.head_ptr->value);
		node* ope_ptr = this->head_ptr;
		this->layer.insert({ ope_ptr->self_layer,{(get<0>(other.layer[1])),ope_ptr} });
		for (int i=2;i<=other.layer_num;i++)
		{
			ope_ptr = get<1>(this->layer[i - 1]);
			while (1)
			{
				if (ope_ptr->sibling == nullptr)
				{
					break;
				}
				if (ope_ptr->has_nodes == 0)
				{
					ope_ptr = ope_ptr->sibling;
				}
				if (ope_ptr->has_nodes != 0)
				{
					break;
				}
			}
			node* temp=get<1>(other.layer[i]);//负责遍历的指针
			node* cat_ptr;
			int64 current_node = 1;
			int64 sibling_times = 0;
			while (1)
			{
				node* add_ptr = new node(temp->has_nodes,temp->self_layer,temp->value);
				ope_ptr->ptr.push_back(add_ptr);
				if (current_node == 1 && sibling_times == 0)
				{
					this->layer.insert({add_ptr->self_layer,{get<0>(other.layer[i]),add_ptr} });
				}
				if (current_node==1&&sibling_times!=0)
				{
					cat_ptr->ptr[cat_ptr->has_nodes - 1]->sibling = ope_ptr->ptr[0];
				}
				else
				{
					if (current_node==1)
					{

					}
					else
					{
						ope_ptr->ptr[current_node-2]->sibling = add_ptr;
					}
				}
				//退出条件
				//1.temp的sibling为空指针
				current_node++;
				if (temp->sibling==nullptr)
				{
					break;
				}
				if (current_node!=ope_ptr->has_nodes+1)
				{
					temp = temp->sibling;
					sibling_times++;
				}
				else
				{
					cat_ptr = ope_ptr;
					while (1)
					{
						if (ope_ptr->sibling==nullptr)
						{
							break;
						}
						if (ope_ptr->has_nodes==0)
						{
							ope_ptr = ope_ptr->sibling;
						}
						if(ope_ptr->has_nodes!=0)
						{
							break;
						}
					}
					current_node = 1;
				}
			}
		}
		this->nodes_sum = other.nodes_sum;
		this->layer_num = other.layer_num;
		tuple<int64, int64>temp = return_ptr(other.ope_ptr);
		this->change_ope(get<0>(temp), get<1>(temp));
		return *this;
	}
	
	vector<T> tree2arr()
	{
		node* ptr_temp = this->ope_ptr;
		vector<T>v;
		for (__int64 i=1;i<=this->layer_num;i++)
		{
			this->ope_ptr = get<1>(this->layer[i]);
			while (this->ope_ptr != nullptr)
			{
				v.push_back(this->ope_ptr->value);
				if (this->ope_ptr->sibling==nullptr)
				{
					break;
				}
				this->ope_ptr=this->ope_ptr->sibling;
			}
		}
		this->ope_ptr = ptr_temp;
		return v;
	}

	void assign_val(T value)
	{
		this->ope_ptr->value = value;
	}

	//get_tree_nodes:最多8000个节点左右
	__int64 get_tree_nodes(node* ptr, __int64 sum)//sum=0算出该节点下的所有节点；sum=1加上本身
	{
		if (ptr->ptr.size() == 0)
		{
			return 0;
		}
		else
		{
			sum += ptr->has_nodes;
			for (__int64 i = 0; i <= ptr->has_nodes - 1; i++)
			{
				sum += get_tree_nodes(ptr->ptr[i], 0);
			}
			return sum;
		}
	}
	__int64 sub_layer_nodes(node* ptr, unsigned __int64 layer_num, __int64 sum)
	{
		static __int64 head_layer = ptr->self_layer;
		if (layer_num > this->layer_num - head_layer)
		{
			try
			{
				throw "out of range!";
			}
			catch (...)
			{
				cout << "out of range!" << endl;
				return;
			}
		}
		else
		{
			if (ptr->self_layer == layer_num + head_layer)
			{
				return 1;
			}
			else
			{
				for (__int64 i = 0; i <= ptr->has_nodes - 1; i++)
				{
					sum += sub_layer_nodes(ptr->ptr[i], layer_num, 0);
				}
				return sum;
			}
		}
	}

	__int64 sub_tree_layer(node* ptr)
	{
		return tail_end(ptr)->self_layer;
	}

	node* tail_end(node* ptr)//return the first branch ptr(without children)
	{
		while (1)
		{
			if (ptr->has_nodes == 0)
			{
				return ptr;
			}
			ptr = ptr->ptr[0];
		}
	}

	node* locate(unsigned __int64 layer_num, unsigned __int64 list_num)//only can find existing nodes
																	   //para:layer_num,list_num
																	   //return: node*
	{
		try
		{
			if (layer_num > this->layer_num)
			{
				throw - 1;
			}
			else
			{
				if (this->sum_nodes(layer_num) + list_num > this->nodes_sum)
				{
					throw - 1;
				}
				else
				{
					node* temp;
					node* layer_ptr = get<1>(this->layer[layer_num]);
					temp = layer_ptr;
					if (temp->sibling == nullptr)
					{
						return temp;
					}
					else
					{
						for (__int64 i = 1; i < list_num; i++)
						{

							temp = temp->sibling;
						}
						return temp;
					}
				}
			}
		}
		catch (...)
		{
			cout << "out of range!" << endl;
		}
	}

	node* locate(unsigned __int64 serial_num)
	{
		try
		{
			if (serial_num > this->nodes_sum)
			{
				throw - 1;
			}
			else
			{
				__int64 sum = 0;
				__int64 i = 1;
				for (i = 1; i < this->layer_num; i++)
				{
					sum = this->sum_nodes(i);
					if (sum >= serial_num)
					{
						break;
					}
				}
				__int64 layer_num = i - 1;
				sum = this->sum_nodes(i - 1);
				__int64 list_num = serial_num - sum;
				locate(layer_num, list_num);
			}
		}
		catch (...)
		{
			cout << "out of range!" << endl;
			return;
		}
	}
	tuple<__int64, __int64> return_ope()
	{
		node* ptr = this->ope_ptr;
		__int64 layer_num = ptr->self_layer;
		__int64 list_num = 0;
		while (ptr != nullptr)
		{
			if (ptr->sibling == nullptr)
			{
				break;
			}
			ptr = ptr->sibling;
			list_num += 1;
		}
		list_num = get<0>(this->layer[ptr->self_layer]) - list_num;
		tuple<__int64, __int64>temp{ layer_num,list_num };
		printf("-----------------------------\n");
		printf(">>ope_ptr\n");
		printf("Layer:%10d\n",layer_num);
		printf("List: %10d\n",list_num);
		printf("Value:%10d\n", this->ope_ptr->value);
		printf("-----------------------------\n");
		return temp;
	}

	node* return_ope_ptr()
	{
		return this->ope_ptr;
	}

	node* return_parent()
	{
		return this->parent_ptr;
	}

	node* return_ancestor()//return ancestor of the ope_ptr
	{
		if (this->ope_ptr==this->parent_ptr)
		{
			return this->ope_ptr;
		}
		else
		{
			node* temp = get<1>(this->layer[this->parent_ptr->self_layer - 1]);
			while (1)
			{
				for (int i=0;i<temp->has_nodes;i++)
				{
					if (temp->ptr[i]==this->parent_ptr)
					{
						return temp;
					}
				}
				temp = temp->sibling;
			}
		}
	}

	node* return_head_ptr()
	{
		return this->head_ptr;
	}

	tuple<__int64, __int64> return_ptr(node* ptr)
	{
		__int64 layer_num = ptr->self_layer;
		__int64 list_num = 0;
		while (ptr != nullptr)
		{
			if (ptr->sibling == nullptr)
			{
				break;
			}
			ptr = ptr->sibling;
			list_num += 1;
		}
		list_num = get<0>(this->layer[ptr->self_layer]) - list_num;
		tuple<__int64, __int64>temp{ layer_num,list_num };
		return temp;
	}

	node* return_front(node* ptr)
	{
		node* previous_ope = this->ope_ptr;
		node* previous_parent = this->parent_ptr;
		tuple<__int64, __int64>temp = this->return_ptr(ptr);
		this->change_ope(get<0>(temp), get<1>(temp));
		if (get<1>(temp) == 1)
		{
			return this->locate(ptr->self_layer - 1, get<0>(this->layer[ptr->self_layer - 1]));
		}
		else
		{
			return this->parent_ptr->ptr[get<1>(temp) - 2];
		}
		this->ope_ptr = previous_ope;
		this->parent_ptr = previous_parent;
	}

	node* return_back(node* ptr)
	{
		if (ptr->sibling!=nullptr)
		{
			return ptr->sibling;
		}
		else
		{
			if (ptr->self_layer==this->layer_num)
			{
				return ptr->sibling;
			}
			else
			{
				return get<1>(this->layer[ptr->self_layer + 1]);
			}
		}
	}

	void describe(node* ptr)
	{
		tuple<__int64, __int64>temp = this->return_ptr(ptr);
		printf("-----------------------------\n");
		printf(">>ptr\n");
		printf("Layer:%10d\n",ptr->self_layer);
		printf("List :%10d\n",get<1>(temp));
		printf("Value:%10d\n", ptr->value);
		printf("-----------------------------\n");
	}

	void describe_tree()
	{
		vector<T>array = this->tree2arr();
		printf("--------------------------------------------\n");
		printf(">>Tree Info:\n");
		printf("Layer number: %-3d\n", this->layer_num);
		printf("Nodes sum: %-7d\n\n", this->nodes_sum);
		printf("--Values listed by serial num:\n");
		int64 sum = 0;
		for (int i=0;i<this->nodes_sum;i++)
		{
			printf("%-7d\t",array[i]);
			sum++;
			if (sum == 5)
			{
				printf("\n");
			}
		}
		printf("\n");
		printf("--------------------------------------------\n\n");
	}
	void jumpto_sibling()
	{
		if (this->ope_ptr->sibling==nullptr)
		{
			try
			{
				throw "error!";
			}
			catch (...)
			{
				cout << "The sibling is nullptr!" << endl;
				return;
			}
		}
		else
		{
			this->change_ope(this->ope_ptr->sibling);
		}
	}

	void jumpto_layerptr(__int64 layer)//choose a layer
	{
		tuple<__int64, __int64>temp = this->return_ptr(get<1>(this->layer[layer]));
		change_ope(get<0>(temp), get<1>(temp));
	}

	void jumpto_layerlast(__int64 layer)//choose a layer
	{
		node* temp_ptr = get<1>(this->layer[layer]);
		while (temp_ptr->sibling!=nullptr)
		{
			temp_ptr = temp_ptr->sibling;
		}
		tuple<__int64, __int64>temp = this->return_ptr(temp_ptr);
		change_ope(get<0>(temp), get<1>(temp));
	}

	void jumpto_layerptr()//default layer is tree`s layer_num
	{
		tuple<__int64, __int64>temp = this->return_ptr(get<1>(this->layer[this->layer_num]));
		change_ope(get<0>(temp), get<1>(temp));
	}

	void jumpto_layerlast()//default layer is tree`s layer_num
	{
		node* temp_ptr = get<1>(this->layer[this->layer_num]);
		while (temp_ptr->sibling!=nullptr)
		{
			temp_ptr = temp_ptr->sibling;
		}
		tuple<__int64, __int64>temp = this->return_ptr(temp_ptr);
		change_ope(get<0>(temp), get<1>(temp));
	}

	void change_ope(unsigned __int64 layer_num, unsigned __int64 list_num)
	//另一种方法：先算出在这层的第几个，再用求子树节点的方法求到是在第几棵子树上（要能只求出这层子树的节点数)
	//change ope_ptr and parent_ptr
	//para:layer_num,list_num
	//usage other method:locate
	{
		
		if (layer_num > this->layer_num)
		{
			throw "out of range!";
		}
		else
		{
			if (this->sum_nodes(layer_num) + list_num > this->nodes_sum)
			{
				throw "out of range!";
			}
			else
			{
				if (layer_num == 1)
				{
					this->ope_ptr = this->head_ptr;
					this->parent_ptr = this->ope_ptr;
				}
				else
				{
					this->ope_ptr = this->locate(layer_num, list_num);
					node* upper_layer_ptr = get<1>(this->layer[layer_num - 1]);
					bool out_of_while = 0;
					while (upper_layer_ptr != nullptr && !out_of_while)
					{
						if (upper_layer_ptr->sibling == nullptr)
						{
							this->parent_ptr = upper_layer_ptr;
							break;
						}
						upper_layer_ptr = upper_layer_ptr->sibling;
						for (__int64 i = 0; i < upper_layer_ptr->ptr.size(); i++)
						{
							if (upper_layer_ptr->ptr[i] == this->ope_ptr)
							{
								this->parent_ptr = upper_layer_ptr;
								out_of_while = 1;
								break;
							}
						}
					}
				}
			}
		}
	}

	void change_ope(node* ptr)
	{
		auto temp=this->return_ptr(ptr);
		change_ope(get<0>(temp), get<1>(temp));
	}

	vector<tuple<__int64, __int64>> find_bfs(T value)//广度搜索
	{
		node* ptr;
		vector<tuple<__int64, __int64>>save_ptrs;
		__int64 num = 0;
		printf("----------------------------------\n");
		printf("BFS Matched nodes:\n");
		printf("%7s  %9s|%-9s\n","Num","Layer","Serial");
		for (__int64 i = 0; i < this->layer_num; i++)
		{
			ptr = get<1>(this->layer[i + 1]);
			__int64 list_num = 1;
			while (ptr != nullptr)
			{
				if (ptr->value == value)
				{
					save_ptrs.push_back({ i + 1,list_num });
				}
				ptr = ptr->sibling;
				list_num += 1;
			}
		}
		for (__int64 i = 0; i < save_ptrs.size(); i++)
		{
			printf("|%6d  (%8d,%-8d)\n", i + 1, get<0>(save_ptrs[i]), get<1>(save_ptrs[i]));
		}
		printf("----------------------------------\n");
		return save_ptrs;
	}

	vector<tuple<__int64, __int64>> find_dfs(T value)//深度搜索
	{
		vector<tuple<__int64, __int64>>save_ptrs;
		printf("----------------------------------\n");
		printf("DFS Matched nodes:\n");
		printf("%7s  %9s|%-9s\n","Num","Layer","Serial");
		if (this->head_ptr->value == value)
		{
			save_ptrs.push_back({ 1,1 });
		}
		find_dfs_branch(save_ptrs, this->head_ptr, value);
		for (__int64 i = 0; i < save_ptrs.size(); i++)
		{
			printf("|%6d  (%8d,%-8d)\n", i + 1, get<0>(save_ptrs[i]), get<1>(save_ptrs[i]));
		}
		printf("----------------------------------\n");
		return save_ptrs;
	}

	__int64 find_child_value(node* ptr,const T& value)//返回父母节点下匹配对应值的孩子节点的位置。
	{
		for (int i=0;i<ptr->ptr.size();i++)
		{
			if (ptr->ptr[i]->value==value)
			{
				return i + 1;
			}
		}
		return -1;
	}

	void add_node(T value, __int64 direction)
	{
		if (direction == 1)//最近操作的节点向下扩展一个节点
		{
			if (!this->ope_ptr->has_nodes)
			{
				this->nodes_sum += 1;
				node* save_ptr = this->parent_ptr;
				node* temp;
				this->parent_ptr = this->ope_ptr;
				this->ope_ptr->has_nodes += 1;
				this->ope_ptr->ptr.push_back(temp);
				this->ope_ptr->ptr[this->ope_ptr->has_nodes - 1] = new node;
				this->ope_ptr->ptr[this->ope_ptr->has_nodes - 1]->value = value;
				this->ope_ptr = this->parent_ptr->ptr[this->parent_ptr->has_nodes - 1];
				if (this->layer.count(this->parent_ptr->self_layer+1))
				{	
					get<0>(this->layer[this->parent_ptr->self_layer+1]) += 1;
					tuple<__int64, __int64>get_pos = return_ptr(this->parent_ptr);
					this->ope_ptr->self_layer = this->layer_num;
					int i = 1;
					while (1)
					{
						if (save_ptr->ptr[get<1>(get_pos) - 1 - i]->ptr.size() == 0)
						{
							i++;
						}
						else
						{
							break;
						}
					}
					save_ptr->ptr[get<1>(get_pos) - i-1]->ptr[(save_ptr->ptr[get<1>(get_pos) - i-1]->has_nodes) - 1]->sibling = this->ope_ptr;
					if (get<1>(get_pos) < save_ptr->has_nodes && save_ptr->ptr[get<1>(get_pos)]->ptr.size() != 0)
					{
						this->ope_ptr->sibling = save_ptr->ptr[get<1>(get_pos)]->ptr[0];
					}
				}
				else
				{
					this->layer_num += 1;
					this->ope_ptr->self_layer = this->parent_ptr->self_layer+1;
					this->layer.insert(pair<__int64, tuple<__int64, node*>>({ this->layer_num, { 1,this->ope_ptr } }));
				}
			}
			else
			{
				try
				{
					throw "error!";
				}
				catch (...)
				{
					cout << "The root already has a child,use direction 0 to add." << endl;
					return;
				}
			}
		}
		else if (direction == 0)
		{
			if (this->parent_ptr->has_nodes!=0)
			{
				this->nodes_sum += 1;
				this->parent_ptr->has_nodes += 1;
				node* temp;
				this->parent_ptr->ptr.push_back(temp);
				this->parent_ptr->ptr[this->parent_ptr->has_nodes - 1] = new node;
				this->parent_ptr->ptr[this->parent_ptr->has_nodes - 1]->value = value;
				this->ope_ptr->sibling = this->parent_ptr->ptr[this->parent_ptr->has_nodes - 1];
				this->ope_ptr = this->parent_ptr->ptr[this->parent_ptr->has_nodes - 1];
				get<0>(this->layer[this->layer_num]) += 1;//改变层总节点数
				this->ope_ptr->self_layer = this->parent_ptr->self_layer + 1;
				if (this->parent_ptr->sibling != nullptr)
				{
					if (this->parent_ptr->sibling->ptr.size() == 0)
					{
						return;
					}
					else
					{
						this->ope_ptr->sibling = this->parent_ptr->sibling->ptr[0];
					}
				}
			}
			else
			{
				try
				{
					throw "error!";
				}
				catch (...)
				{
					cout << "The root already has no child!direction 0 must use with no child root." << endl;
					return;
				}
			}
		}
		else if (direction==-1)
		{
			if (this->parent_ptr != this->ope_ptr)
			{
				this->change_ope(this->parent_ptr);
				this->change_ope(this->parent_ptr);
				node* new_ptr = new node(0,value);
				new_ptr->self_layer = this->ope_ptr->self_layer + 1;
				this->ope_ptr->ptr.push_back(new_ptr);
				this->ope_ptr->has_nodes++;
				get<0>(this->layer[this->ope_ptr->ptr[this->ope_ptr->has_nodes - 1]->self_layer])++;
				this->ope_ptr->ptr[this->ope_ptr->has_nodes - 2]->sibling = this->ope_ptr->ptr[this->ope_ptr->has_nodes - 1];
				this->nodes_sum++;
				this->change_ope(new_ptr);
			}
			else			
			{
				node* temp_head = this->head_ptr;
				this->head_ptr = new node(1, value);
				this->head_ptr->ptr.push_back(temp_head);
				this->layer_num++;
				this->nodes_sum++;
				this->layer.insert({ this->layer_num,{get<0>(this->layer[this->layer_num-1]),get<1>(this->layer[this->layer_num-1])} });
				__int64 temp_former = get<0>(this->layer[1]);
				node* ptr_temp_former = get<1>(this->layer[1]);
				get<0>(this->layer[1]) = 1;
				get<1>(this->layer[1]) = this->head_ptr;
				__int64 count = 2;
				__int64 temp_lat;
				node* ptr_temp_lat;
				while (count!=this->layer_num)
				{
					temp_lat = get<0>(this->layer[count]);
					ptr_temp_lat = get<1>(this->layer[count]);
					get<0>(this->layer[count]) = temp_former;
					get<1>(this->layer[count]) = ptr_temp_former;
					count++;
					temp_former = temp_lat;
					ptr_temp_former = ptr_temp_lat;
				}
				for (__int64 i = 2; i <= this->layer_num; i++)
				{
					get<1>(this->layer[i])->self_layer++;
					node* ope_temp = get<1>(this->layer[i]);
					while (1)
					{
						if (ope_temp->sibling == nullptr)
						{
							break;
						}
						ope_temp = ope_temp->sibling;
						ope_temp->self_layer++;
					}
				}
				this->parent_ptr = this->head_ptr;
				this->ope_ptr = this->head_ptr;
			}
		}
		else
		{
			try
			{
				throw "error!";
			}
			catch (...)
			{
				cout << "error add mode!" << endl;
				return;
			}
		}
	}
	void insert(__int64 serial_num, T value)
	{
		if (serial_num > this->nodes_sum)
		{
			throw "out of range!";
		}
		else
		{
			__int64 sum = 0;
			__int64 i = 1;
			for (i = 1; i < this->layer_num; i++)
			{
				sum = this->sum_nodes(i);
				if (sum >= serial_num)
				{
					break;
				}
			}
			__int64 layer_num = i - 1;
			sum = this->sum_nodes(i - 1);
			__int64 list_num = serial_num - sum;
			insert(layer_num, list_num, value);
		}
	}

	void insert(__int64 layer_num, __int64 list_num, T value)
	{
		if (layer_num > this->layer_num)
		{
			throw "out of range";
		}
		else
		{
			if (list_num > get<0>(this->layer[layer_num]))
			{
				throw "out of range";
			}
			else
			{
				node* moving_ptr = get<1>(this->layer[layer_num]);
				node* insert_ptr = new node;
				if (list_num == 1)
				{
					insert_ptr->sibling = moving_ptr;
					insert_ptr->self_layer = layer_num;
					insert_ptr->value = value;
					get<1>(this->layer[layer_num]) = insert_ptr;
					get<0>(this->layer[layer_num]) += 1;
					this->change_ope(layer_num, list_num);
					this->parent_ptr->ptr.insert(this->parent_ptr->ptr.begin(), insert_ptr);
					this->parent_ptr->has_nodes += 1;
					cout << endl;
					this->return_ptr(this->parent_ptr);
				}
				else
				{
					for (__int64 i = 0; i < list_num - 2; i++)
					{
						moving_ptr = moving_ptr->sibling;
					}
					node* next_ptr = moving_ptr->sibling;
					moving_ptr->sibling = insert_ptr;
					insert_ptr->sibling = next_ptr;
					insert_ptr->self_layer = layer_num;
					insert_ptr->value = value;
					get<0>(this->layer[layer_num]) += 1;
					this->change_ope(layer_num, list_num);
					this->parent_ptr->ptr.insert(this->parent_ptr->ptr.begin() + list_num - 1, insert_ptr);
					this->parent_ptr->has_nodes += 1;
				}
			}
		}
	}

	void del_node()//delete ope_ptr`s node problem
	{
		this->parent_ptr->has_nodes -= 1;
		__int64 i = 0;
		for (i = 0; i < this->parent_ptr->ptr.size(); i++)
		{
			if (this->parent_ptr->ptr[i] == this->ope_ptr)
			{
				break;
			}
		}
		this->nodes_sum -= 1;
		get<0>(this->layer[this->ope_ptr->self_layer]) -= 1;
		if (this->parent_ptr->ptr.size() == 1)//单节点拼接上去
		{
			for (__int64 j = 0; j < this->ope_ptr->ptr.size(); j++)
			{
				this->parent_ptr->ptr.push_back(this->ope_ptr->ptr[j]);
				this->parent_ptr->has_nodes++;
			}
			this->layer.erase(this->ope_ptr->self_layer);
			this->layer_num -= 1;
			map<__int64, tuple<__int64, node*>>temp;
			for (__int64 j = 1; j < this->ope_ptr->self_layer; j++)
			{
				temp.insert(pair<__int64, tuple<__int64, node*>>(j, this->layer[j]));
			}
			for (__int64 j = this->ope_ptr->self_layer; j <= this->layer_num; j++)
			{
				temp.insert(pair<__int64, tuple<__int64, node*>>(j, { get<0>(this->layer[j + 1]), get<1>(this->layer[j + 1]) }));
			}
			this->layer = temp;
			delete this->ope_ptr;
			tuple<__int64, __int64>temp_ope_ptr = this->return_ptr(parent_ptr);
			this->change_ope(get<0>(temp_ope_ptr), get<1>(temp_ope_ptr));
		}
		else//节点下所有连接节点全部被转交给左边的兄弟节点
		{
			if (this->parent_ptr->ptr.size() < i + 2)
			{
				this->parent_ptr->ptr[i - 1]->sibling = nullptr;
			}
			else
			{
				this->parent_ptr->ptr[i - 1]->sibling = this->parent_ptr->ptr[i + 1];
			}
			for (__int64 j = 0; j < this->ope_ptr->ptr.size(); j++)
			{
				this->parent_ptr->ptr[i - 1]->ptr.push_back(this->ope_ptr->ptr[j]);
				this->parent_ptr->ptr[i - 1]->has_nodes++;
			}
			delete this->ope_ptr;
			tuple<__int64, __int64>temp_ope_ptr = this->return_ptr(parent_ptr);
			this->change_ope(get<0>(temp_ope_ptr), get<1>(temp_ope_ptr));
		}
	}

	void del_node(unsigned __int64 layer_num, unsigned __int64 list_num)
	{
		if (layer_num > this->layer_num)
		{
			throw "out of range!";
		}
		else
		{
			if (list_num > get<0>(this->layer[layer_num]))
			{
				throw "out of range!";
			}
			else
			{
				node* temp = this->locate(layer_num, list_num);
				if (temp == this->ope_ptr)
				{
					del_node();
				}
				else
				{
					tuple<__int64, __int64>temp_tuple = this->return_ope();
					this->change_ope(layer_num, list_num);
					del_node();
					this->change_ope(get<0>(temp_tuple), get<1>(temp_tuple));
				}
			}
		}
	}
	void del_node(unsigned __int64 serial_num)
	{
		if (serial_num > this.nodes_sum)
		{
			throw "out of range!";
		}
		else
		{
			tuple<__int64, __int64>temp_tuple = this->return_ptr(this->locate(serial_num));
			del_node(get<0>(temp_tuple), get<1>(temp_tuple));
		}
	}

	void del_ptr_tree(node* ptr)//problem
	{
		if (sub_tree_layer(ptr) == this->layer_num)
		{
			__int64 layer = ptr->self_layer;
			node* temp = ptr;
			while (1)
			{
				if (get<1>(this->layer[layer]) == temp)
				{
					break;
				}
				temp = temp->ptr[0];
				layer++;
				if (layer == this->layer_num+1)
				{
					break;
				}
			}
			if (layer==this->layer_num+1)
			{
				return;
			}
			else
			{
				for (int i=layer;i<=this->layer_num;i++)
				{
					this->layer.erase(i);
				}
			}
		}
		else
		{
			if (ptr==get<1>(this->layer[ptr->self_layer]))
			{
				if (ptr->sibling!=nullptr)
				{

				}
				for (int i=ptr->self_layer;i<=this->layer_num;i++)
				{
					this->layer.erase(i);
				}
			}
		}
	}

	//clear:最多7000个节点
	void clear()
	{
		this->del_node_clear();
		this->ope_ptr = this->head_ptr;
		this->parent_ptr = this->head_ptr;
		this->nodes_sum = 1;
		this->layer_num = 1;
		this->layer.clear();
		this->layer.insert(pair<__int64, tuple<__int64, node*>>(1, { 1, head_ptr }));
	}

	void route_front(string name)
	{
		printf("-----------------------------------------------------------------------------------\n");
		printf(">>Tree %s lists:\n",name);
		printf("    Layer|Serial\n");
		printf("(       1,1       )  ||  value=%-20f\n",this->head_ptr->value);
		route_front_branch(head_ptr);
		printf("-----------------------------------------------------------------------------------\n");
	}

	void route_front()
	{
		printf("-----------------------------------------------------------------------------------\n");
		printf("    Layer|Serial\n");
		printf("(       1,1       )  ||  value=%-20f\n",this->head_ptr->value);
		route_front_branch(head_ptr);
		printf("-----------------------------------------------------------------------------------\n");
	}

	void route_back(string name)
	{
		printf("-----------------------------------------------------------------------------------\n");
		printf(">>Tree %s lists:\n",name);
		printf("    Layer|Serial\n");
		printf("(       1,1       )  ||  value=%-20f\n", this->head_ptr->value);
		route_back_branch(head_ptr);
		printf("-----------------------------------------------------------------------------------\n");
	}

	void route_back()
	{
		printf("-----------------------------------------------------------------------------------\n");
		printf("    Layer|Serial\n");
		printf("(       1,1       )  ||  value=%-20f\n", this->head_ptr->value);
		route_back_branch(head_ptr);
		printf("-----------------------------------------------------------------------------------\n");
	}

	void route_front_assign(node* ptr,__int64 layer)
	{
		route_front_assign_branch(ptr,layer);
	}

	void route_back_assign(node* ptr,__int64 layer)
	{
		route_back_assign_branch(ptr,layer);
	}

	void exhaust_all()//exhausion
	{

	}

	void draw_tree()
	{


	}

protected:
	node* head_ptr;
	node* ope_ptr;
	node* parent_ptr;//和最后操作指针一齐变动,做私有数据
	map<__int64, tuple<__int64, node*>>layer;//第一个__int64：第几层；第二个__int64:这层的节点数量；第三个node*:指向层首节点
	void del_node_clear()
	{
		static __int64 layer_num = this->layer_num;
		if (layer_num == 1)
		{
			return;
		}
		this->ope_ptr = get<1>(this->layer[layer_num]);
		for (__int64 i = 1; i <= get<0>(this->layer[layer_num]); i++)
		{
			node* temp = this->ope_ptr->sibling;
			delete this->ope_ptr;
			this->ope_ptr = temp;
		}
		layer_num -= 1;
		del_node_clear();
	}

	void find_dfs_branch(vector<tuple<__int64, __int64>>& save_ptrs, node* ptr, T value)
	{
		if (ptr->has_nodes == 0)
		{
			return;
		}
		else
		{
			for (__int64 i = 0; i < ptr->has_nodes; i++)
			{
				if (ptr->ptr[i]->value == value)
				{
					save_ptrs.push_back(this->return_ptr(ptr->ptr[i]));
				}
				find_dfs_branch(save_ptrs, ptr->ptr[i], value);
			}
		}
	}

	void route_front_branch(node* ptr)
	{
		if (ptr->has_nodes == 0)
		{
			return;
		}
		else
		{

			tuple<__int64, __int64>temp_parent = this->return_ptr(ptr);
			for (__int64 i = 0; i < ptr->has_nodes; i++)
			{
				tuple<__int64, __int64>temp = this->return_ptr(ptr->ptr[i]);
				printf("(%8d,%-8d)  ||  value=%-15f  ||  parent(%8d,%-8d)\n", get<0>(temp), get<1>(temp), ptr->ptr[i]->value, get<0>(temp_parent), get<1>(temp_parent));
				this->route_front_branch(ptr->ptr[i]);
			}
		}
	}

	void route_front_assign_branch(node* ptr,__int64 layer)
	{
		if (ptr->has_nodes == 0)
		{
			return;
		}
		else
		{
			for (__int64 i = 0; i < ptr->has_nodes; i++)
			{
				ptr->ptr[i]->self_layer = layer;
				this->route_front_assign_branch(ptr->ptr[i],layer+1);
			}
		}
	}


	void route_back_branch(node* ptr)
	{
		if (ptr->has_nodes == 0)
		{
			return;
		}
		else
		{

			tuple<__int64, __int64>temp_parent = this->return_ptr(ptr);
			for (__int64 i = ptr->has_nodes - 1; i != 0; i--)
			{
				tuple<__int64, __int64>temp = this->return_ptr(ptr->ptr[i]);
				printf("(%8d,%-8d)  ||  value=%-15f  ||  parent(%8d,%-8d)\n", get<0>(temp), get<1>(temp), ptr->ptr[i]->value, get<0>(temp_parent), get<1>(temp_parent));
				this->route_back_branch(ptr->ptr[i]);
			}
		}
	}

	void route_back_assign_branch(node* ptr,__int64 layer)
	{
		if (ptr->has_nodes == 0)
		{
			return;
		}
		else
		{
			for (__int64 i = ptr->has_nodes - 1; i != 0; i--)
			{
				ptr->ptr[i]->self_layer = layer;
				this->route_back_assign_branch(ptr->ptr[i],layer+1);
			}
		}
	}

private:
};

template <typename T> class binary_tree :public tree<T>
{
	template <typename T>
	friend binary_tree<T> add_tree(const binary_tree<T> &tree_added,const binary_tree<T> &tree_toadd);
public:

	binary_tree() :tree<T>() {};
	binary_tree(T value) :tree<T>(value) {};
	binary_tree(T* array, __int64 n) :tree<T>(array, 2, n) {};
	~binary_tree() {};

	//binary_tree& operator=(binary_tree& other)
	//{
	//	
	//}
	void add_node(T value,__int64 direction)//限制每个节点下最多添加两个子节点，超出不会提醒错误
	{
		if (direction == 1)//最近操作的节点向下扩展一个节点
		{
			if (this->ope_ptr->has_nodes == 2)
			{
				throw "out of range!";
			}
			else
			{
			this->nodes_sum += 1;
			this->parent_ptr = this->ope_ptr;
			this->ope_ptr->has_nodes += 1;
			node* temp;
			this->ope_ptr->ptr.push_back(temp);
			this->ope_ptr->ptr[this->ope_ptr->has_nodes - 1] = new node;
			this->ope_ptr->ptr[this->ope_ptr->has_nodes - 1]->value = value;
			this->ope_ptr = this->parent_ptr->ptr[this->parent_ptr->has_nodes - 1];
			this->layer_num += 1;
			this->ope_ptr->self_layer = this->layer_num;
			this->layer.insert(pair<__int64, tuple<__int64, node*>>(this->layer_num, { 1,this->ope_ptr }));
			}
		}
		else if (direction == 0)
		{
			if (this->parent_ptr == this->ope_ptr)
			{
				try
				{
					throw "error!This is the root node!";
				}
				catch (...)
				{
					cout << "error!This is the root node!" << endl;
					return;
				}
			}
			else
			{
				node* memo_ptr = this->parent_ptr;
				node* neighbor_ptr = this->parent_ptr;
				if (this->parent_ptr->has_nodes == 2)
				{
					__int64 added_node = 0;
					while (this->parent_ptr->sibling != nullptr && added_node == 0)
					{
						if (this->parent_ptr->sibling->has_nodes < 2)
						{
							added_node++;
							neighbor_ptr = this->parent_ptr;
							this->parent_ptr = this->parent_ptr->sibling;
							this->nodes_sum += 1;
							this->parent_ptr->has_nodes++;
							node* temp;
							this->parent_ptr->ptr.push_back(temp);
							this->parent_ptr->ptr[this->parent_ptr->has_nodes - 1] = new node;
							this->parent_ptr->ptr[this->parent_ptr->has_nodes - 1]->value = value;
							if (this->parent_ptr->has_nodes == 1)
							{
								neighbor_ptr->ptr[neighbor_ptr->has_nodes - 1]->sibling = this->parent_ptr->ptr[0];
							}
							else if (this->parent_ptr->has_nodes == 2)
							{
								this->parent_ptr->ptr[this->parent_ptr->has_nodes - 2]->sibling = this->parent_ptr->ptr[this->parent_ptr->has_nodes - 1];
							}
							this->ope_ptr = this->parent_ptr->ptr[this->parent_ptr->has_nodes - 1];
							get<0>(this->layer[this->layer_num]) += 1;//改变层总节点数
							this->ope_ptr->self_layer = this->layer_num;
							this->parent_ptr = memo_ptr;
						}
						else if (this->parent_ptr->sibling->has_nodes == 2)
						{
							this->parent_ptr = this->parent_ptr->sibling;
						}
						else if (this->parent_ptr->sibling == nullptr)
						{
							try
							{
								this->parent_ptr = memo_ptr;
								throw "out of range!";
							}
							catch (...)
							{
								cout << "out of range!" << endl;
								return;
							}
						}
					}
				}
				else
				{
					this->nodes_sum += 1;
					this->parent_ptr->has_nodes++;
					node* temp;
					this->parent_ptr->ptr.push_back(temp);
					this->parent_ptr->ptr[this->parent_ptr->has_nodes - 1] = new node;
					this->parent_ptr->ptr[this->parent_ptr->has_nodes - 1]->value = value;
					if (this->parent_ptr->has_nodes - 1 == 1)
					{
						get<1>(this->layer[this->layer_num])->sibling = this->parent_ptr->ptr[this->parent_ptr->has_nodes - 1];
					}
					else
					{
						this->ope_ptr->sibling = this->parent_ptr->ptr[this->parent_ptr->has_nodes - 1];
					}
					this->ope_ptr = this->parent_ptr->ptr[this->parent_ptr->has_nodes - 1];
					get<0>(this->layer[this->layer_num]) += 1;//改变层总节点数
					this->ope_ptr->self_layer = this->layer_num;
				}
			}
		}
		else
		{
			try
			{
				throw "error!";
			}
			catch (...)
			{
				cout << "error add mode!" << endl;
				return;
			}
		}
	}
	node* left_child(node* ptr)
	{
		if (ptr->has_nodes == 0)
		{
			try
			{
				throw "error!";
			}
			catch (...)
			{
				cout << "This node doesn`t have left child." << endl;
			}
		}
		else
		{
			return ptr->ptr[0];
		}
	}
	node* right_child(node* ptr)
	{
		if (ptr->has_nodes == 0 || ptr->has_nodes == 1)
		{
			try
			{
				throw "error!";
			}
			catch (...)
			{
				cout << "This node doesn`t have right child." << endl;
			}
		}
		else
		{
			return ptr->ptr[1];
		}
	}

	void node_reverse(node* ptr)
	{
		if (ptr->has_nodes!=2)
		{
			try
			{
				throw "error!";
			}
			catch (...)
			{
				cout << "This node doesn`t have two nodes." << endl;
				return;
			}
		}
		else
		{
				ptr->ptr.insert(ptr->ptr.begin(), ptr->ptr[1]);
				get<1>(tree_added.layer[ptr->ptr[0]->self_layer]) = ptr->ptr[0];
				node* temp = ptr->ptr[2]->sibling;
				ptr->ptr[0]->sibling = ptr->ptr[1];
				ptr->ptr.pop_back();
				ptr->ptr[1]->sibling = temp;
		}
	}
protected:
};

template <typename T>//严重的污染参数问题
	binary_tree<T> add_tree(const binary_tree<T>&tree_first,const binary_tree<T>&tree_second)
	{	
			binary_tree<T>tree_added = tree_first;
			binary_tree<T>tree_toadd = tree_second;
			tree_added.ope_ptr->ptr.push_back(tree_toadd.head_ptr);
			tree_added.ope_ptr->has_nodes++;
			tree<T>::node* temp_head = tree_added.ope_ptr->ptr[0];
			map<__int64, tuple<__int64, tree<T>::node*>>temp_map;
			for (int i=1;i<=tree_toadd.layer_num;i++)
			{
				bool not_to_add = 0;
				temp_map.insert({ i,{get<0>(tree_toadd.layer[i]),temp_head} });
				while (1)
				{
					if (temp_head->ptr.size()==0)
					{
						if (temp_head->sibling == nullptr)
						{
							not_to_add = 1;
							break;
						}
						else
						{
							temp_head = temp_head->sibling;
						}
					}
					else
					{
						temp_head = temp_head->ptr[0];
						break;
					}
				}
				if (not_to_add==1)
				{
					break;
				}
			}
			tree_added.nodes_sum += tree_toadd.nodes_sum;
			tree_added.route_front_assign(tree_added.ope_ptr->ptr[0], tree_added.ope_ptr->self_layer+2);
			if (tree_added.ope_ptr->has_nodes == 1)//添加树的节点只有一个时，这时所有树的行指针都要改变
			{
				if (tree_added.ope_ptr==get<1>(tree_added.layer[tree_added.ope_ptr->self_layer]))//如果被操作指针是行指针
				{
					if (tree_added.ope_ptr->self_layer+tree_toadd.layer_num>= tree_added.layer_num)
					{
						for (int i = tree_added.ope_ptr->self_layer + 1; i <= tree_added.layer_num; i++)
						{
							get<1>(tree_added.layer[i]) = get<1>(temp_map[i - tree_added.ope_ptr->self_layer]);
							get<1>(tree_added.layer[i])->self_layer = i;
							get<0>(tree_added.layer[i]) = get<0>(tree_added.layer[i]) + get<0>(temp_map[i - tree_added.ope_ptr->self_layer]);
						}
						for (int i = tree_added.layer_num + 1; i <= tree_added.ope_ptr->self_layer + tree_toadd.layer_num; i++)
						{
							tree_added.layer.insert({ i, {get<0>(temp_map[i - tree_added.ope_ptr->self_layer]) ,get<1>(temp_map[i - tree_added.ope_ptr->self_layer]) } });
							get<1>(tree_added.layer[i])->self_layer = i;
						}
						tree_added.layer_num = tree_added.ope_ptr->self_layer + tree_toadd.layer_num;
					}
					else
					{
						for (int i = tree_added.ope_ptr->self_layer + 1; i <= tree_added.ope_ptr->self_layer+tree_toadd.layer_num; i++)
						{
							get<1>(tree_added.layer[i]) = get<1>(temp_map[i - tree_added.ope_ptr->self_layer]);
							get<1>(tree_added.layer[i])->self_layer = i;
							get<0>(tree_added.layer[i]) = get<0>(tree_added.layer[i]) + get<0>(temp_map[i - tree_added.ope_ptr->self_layer]);
						}
					}
				}
				else
				{
					if (tree_added.ope_ptr->self_layer+tree_toadd.layer_num>tree_added.layer_num)
					{
						for (int i = tree_added.layer_num + 1; i <= tree_added.ope_ptr->self_layer + tree_toadd.layer_num; i++)
						{
							tree_added.layer.insert({ i, { get<0>(temp_map[i - tree_added.ope_ptr->self_layer]) ,get<1>(temp_map[i - tree_added.ope_ptr->self_layer]) } });
							get<1>(tree_added.layer[i])->self_layer = i;
						}
						tree_added.layer_num = tree_added.ope_ptr->self_layer + tree_toadd.layer_num;
					}
				}
			}
			else if (tree_added.ope_ptr->has_nodes == 2)
			{
				if (tree_toadd.layer_num + tree_added.ope_ptr->self_layer > tree_added.layer_num)
				{
					for (int i = tree_added.layer_num + 1; i <= tree_added.ope_ptr->self_layer + tree_toadd.layer_num; i++)
					{
						tree_added.layer.insert({ i, { get<0>(temp_map[i - tree_added.layer_num + 1]),get<1>(temp_map[i - tree_added.layer_num + 1]) } });
						get<1>(tree_added.layer[i])->self_layer = i;
					}
					tree_added.layer_num = tree_added.ope_ptr->self_layer + tree_toadd.layer_num;
				}
			}
			//处理sibling连接的代码
			while (1)
			{
				tree<T>::node* toadd_node;
				tree<T>::node* added_node = tree_added.ope_ptr;
				if (added_node->sibling == nullptr)
				{
					tree<T>::node* row_ptr = get<1>(tree_added.layer[tree_added.layer_num]);
					while (1)
					{
						if (row_ptr->sibling == nullptr)
						{
							break;
						}
						row_ptr = row_ptr->sibling;
					}
					tree_added.change_ope(row_ptr);//可在上面的while循环里节约性能
					return tree_added;
				}
				else
				{
					int temp_layer = tree_added.ope_ptr->self_layer + 1;
					while (1)
					{
						added_node = added_node->sibling;
						if (added_node->has_nodes == 0)
						{
							if (added_node->sibling == nullptr)
							{
								break;
							}
							else
							{
								added_node = added_node->sibling;
							}
						}
						tree<T>::node* temp_sibling_ptr = added_node;
						while (added_node->ptr.size() != 0)
						{
							added_node = added_node->ptr[0];
							if (added_node->self_layer == temp_layer)
							{
								toadd_node = tree_added.ope_ptr->ptr[tree_added.ope_ptr->has_nodes - 1];
								while (toadd_node->ptr.size() != 0)
								{
									if (toadd_node->self_layer == temp_layer)
									{
										toadd_node->sibling = added_node;
										temp_layer++;
										break;
									}
									toadd_node = toadd_node->ptr[toadd_node->has_nodes - 1];
								}
								if (added_node->ptr.size() == 0)
								{
									break;
								}
							}
						}
						if (temp_layer == tree_added.layer_num)
						{
							break;
						}
						added_node = temp_sibling_ptr;
					}
				}
			}
	}

#endif
