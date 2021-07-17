#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>


template <typename Key, size_t N = 7>

class ADS_set {

public:
	class Iterator;
	using value_type = Key;
	using key_type = Key;
	using reference = key_type&;
	using const_reference = const key_type&;
	using size_type = size_t;
	using difference_type = std::ptrdiff_t;
	using iterator = Iterator;
	using const_iterator = Iterator;
	//using key_compare = std::less<key_type>; // for B+-Tree
	using key_equal = std::equal_to<key_type>; // Hashing
	using hasher = std::hash<key_type>;        // Hashing

private:

	/* a node is represented by the key_type element, a pointer to the next element and a head pointer. */
	struct node {
		
		key_type key; 		   //element in list         
		node* next = nullptr;  //ptr to next elem in list
		node* head = nullptr;  //head ptr
		~node() {
			delete head;
			delete next;
		}
	};

	node* table{ nullptr };     //ptr to first element in table
	size_type table_size{ 0 };  //capacity
	size_type curr_size{ 0 };   //actual size
	float max_lf{ 0.75 };       //max load factor

	size_type h(const_reference key) const { return hasher{}(key) % table_size; }  //hash method

	node* insert_(const_reference key);
	
	node* find_(const_reference key) const;

	void rehash(size_type n);

	void reserve();

public:

	/* ************ */
	/* CONSTRUCTORS */
	/* ************ */


	/* default constructor => O(1) */
	ADS_set() { rehash(N); }

	/* initiaizer-list constructor => O(ilist size) */
	ADS_set(std::initializer_list<key_type> ilist) : ADS_set{} {
		
		insert(ilist);
	}

	/* range constructor => O(range size) */
	template<typename InputIt> ADS_set(InputIt first, InputIt last) : ADS_set{} {

		insert(first, last);
	}
	
	/* copy-constructor => O(other size) */
	ADS_set (const ADS_set &other) : ADS_set{}{
		
		for(const auto& elem : other)
			insert_(elem);
	}


	
	/* ********** */
	/* DESTRUCTOR */
	/* ********** */


	/* destructor => O(size) */
	~ADS_set(){ delete[] table; };



	/* ******************* */
	/* ASSIGNMENT OPERATOR */
	/* ******************* */


	/* operator= */
	ADS_set &operator=(const ADS_set &other){												
		
		clear();
	
		for(const auto& elem : other)
			insert(elem);
	
		return *this;
	}
	
	/* operator= */				
  	ADS_set &operator=(std::initializer_list<key_type> ilist){										

		clear();
		insert(ilist);
	
		return *this;
	}



	/* **** */
	/* SIZE */
	/* **** */


	/* returns actual capacity => O(1) */
	size_type size() const {														

		return curr_size;
	}

	/* checks if table is empty => O(1) */
	bool empty() const {															

		return curr_size == 0;
	}



	/* ****** */
	/* INSERT */
	/* ****** */


	/* inserts from list O(ilist size) */
	void insert(std::initializer_list<key_type> ilist) {											

		for (auto &elem : ilist)
		{
          		if (!count(elem))
			{
                		insert_(elem);
			}
		}
	}

	/* inserts key => O(1) */
	std::pair<iterator, bool> insert(const_reference key){											

		if(!count(key))
		{
			insert_(key);

			return std::make_pair(find(key), true);
		}
		return std::make_pair(find(key), false);
	}
	
	/* insert from range => O(range size) */
	template<typename InputIt> void insert(InputIt first, InputIt last) {									

		for(auto elem{first}; elem != last; ++elem)
		{
			if(!count(*elem))
			{
				insert_(*elem);
			}
		}
	}



	/* ****** */
	/* DELETE */
	/* ****** */
	
	
	/* deletes all elements => O(size) */
	void clear(){																

		delete[] table;
		curr_size = 0;
		table_size = N;
		table = new node[table_size];
	}
	
	/* deletes one element => O(1) */
	size_type erase(const_reference key){													

		if(count(key))
		{
			auto idx = h(key);  //sets idx at the head of the list where key is contained
		
			node* elem_curr = table[idx].head;  //1st elem from table
			node* elem_prev = nullptr;  		//prev elem (for now NULL)
			
			for(; elem_curr != nullptr; elem_curr = elem_curr->next)
			{
				if(key_equal{}(elem_curr->key, key))  //compares the current element with the element to be deleted
				{
					if(elem_curr == table[idx].head)  //if curr = 1st elem from a chain/list
					{
						table[idx].head = elem_curr->next;  //head = ptr to the next elem (of the element to be deleted)
						elem_curr->next = nullptr;  		//ptr to next (of the deleted element) set on NULL
						delete elem_curr;  					//delete elem
						--curr_size;
						return 1;
					}
					if(elem_prev)  //if there is a prev elem
					{
						elem_prev->next = elem_curr->next;  //ptr of prev elem becomes ptr to the next elem (of the lement to be deleted)
						elem_curr->next = nullptr;  		
						delete elem_curr;  					
						elem_prev = nullptr;
						delete elem_prev;
						--curr_size;
						return 1;
					}
				}
				elem_prev = elem_curr;
			}
		}
		return 0;
	}



	/* ****** */
	/* SEARCH */
	/* ****** */


	/* tests if an elem exists in table => O(1) */
	size_type count(const_reference key) const {												

		const auto idx = h(key);
		for (node* n = table[idx].head /*ptr to 1st element in list*/; n != nullptr; n = n->next)
			if (key_equal{}(n->key, key))
				return 1;

		return 0;
	}			
	
	/* returns iterator on elem => O(1)	*/
	iterator find(const_reference key) const{												

		const auto idx = h(key);
		for(node* it = table[idx].head; it != nullptr; it = it->next)
			if(key_equal{}(it->key, key))
				return const_iterator{this, it, idx, table_size};

		return end();
	}



	/* ****************** */
	/* OTHER MODIFICATONS */
	/* ****************** */


	/* swap => O(1) */
	void swap(ADS_set &other){														

		std::swap(table_size, other.table_size);
		std::swap(table, other.table);
		std::swap(curr_size, other.curr_size);
	}



	/* **************** */
	/* ITERATOR METHODS */
	/* **************** */


	/* first elem */
  	const_iterator begin() const{														

		for(size_type it{0}; it < table_size; ++it)
			if(table[it].head)
				return const_iterator(this, table[it].head, it, table_size);
		return end();
	}

	/* virtual iterator after the last elem (NULL) */
  	const_iterator end() const{														
	
		return const_iterator(nullptr);
	}



	/* ********* */
	/* DEBUGGING */
	/* ********* */


	/* cout method */
	void dump(std::ostream& o = std::cerr) const {												

		o << "curr_size = " << curr_size << " " << "table_size = " << table_size << std::endl;	//optional	
		
		
		for (size_type it{ 0 }; it < table_size; ++it)
		{
			o << "[" << it << "]" << ": ";
			if (table[it].head == nullptr)
			{
				o << "free" << std::endl;
				continue;
			}

			for (node* n = table[it].head; n != nullptr; n = n->next)
			{
				o << n->key;
				if (n->next != nullptr)
					o << " -> ";
			}
			o << std::endl;
		}
	}



	/* compares two tables => O(size) */
	friend bool operator==(const ADS_set &lhs, const ADS_set &rhs){	
										
		if(lhs.curr_size != rhs.curr_size)
		return false;
	
		for(const auto& elem : lhs)
			if(!rhs.count(elem))
				return false;
			
		return true;
	}

	/* compares two tables => O(size) */
  	friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs){										
		
		return !(lhs == rhs);
	}
};


/* rehash */
template <typename Key, size_t N>
void ADS_set<Key, N>::rehash(size_type n) {

	std::vector<Key> vect;
	vect.reserve(curr_size);
	
	for (size_type i{ 0 }; i < table_size; ++i)
		for(node* n = table[i].head; n != nullptr; n = n->next)
			vect.push_back(n->key);	

	delete[] table;
	table_size = n;
	curr_size = 0;
	
	table = new node[table_size];
	for(const auto& elem : vect)
		insert_(elem);
}

/*reserve */
template <typename Key, size_t N>
void ADS_set<Key, N>::reserve(){
	
	if(table_size * 2 < curr_size)
	//if(curr_size / table_size > max_lf)
	{
		size_type new_table_size{table_size};
		do
		{
			new_table_size = new_table_size * 2 + 1;
		}while(new_table_size * 2 < curr_size);  //table_size * 2 < curr_size
		rehash(new_table_size);
	}
}

/* insert */
template <typename Key, size_t N>
typename ADS_set<Key, N>::node* ADS_set<Key, N>::insert_(const key_type& key){

	node* new_n = new node;
	auto idx{h(key)};

	new_n->next = table[idx].head;
	new_n->key = key;
	table[idx].head = new_n;
	++curr_size;

	if(table_size * 2 + 1 < curr_size)
		reserve();			

	return table + idx;

}

/* find */
template <typename Key, size_t N>
typename ADS_set<Key, N>::node* ADS_set<Key, N>::find_(const key_type& key) const{

	auto idx{h(key)};

	for(size_type i{0}; i < table_size; ++i)
		for(node* n = table[idx].head; n != nullptr; n = n->next)
		{
			if(key_equal{}(n->key, key))
				return n->key;
		}

	return table + curr_size;
}
		
/* swap */
template <typename Key, size_t N> void swap(ADS_set<Key, N>& lhs, ADS_set<Key, N>& rhs) { 

	lhs.swap(rhs); 
}



	/* ******** */
	/* ITERATOR */
	/* ******** */


template <typename Key, size_t N>
class ADS_set<Key, N>::Iterator {

private:
	const ADS_set<Key, N> *ptr;  //ptr to this table
	node* n;  					 //actual element
	size_type idx;  			 //index
	size_type sz;  				 //table_size

public:	
	using value_type = Key;
	using difference_type = std::ptrdiff_t;
	using reference = const value_type&;
	using pointer = const value_type*;
	using iterator_category = std::forward_iterator_tag;

	//ctor
	explicit Iterator(const ADS_set *ptr = nullptr, node* n = nullptr, size_type idx = 0, size_type sz = 0) : ptr{ptr}, n{n}, idx{idx}, sz{sz}{}	

	//reference
	reference operator*() const{																

		return n->key;
	}

	//pointer
	pointer operator->() const{																
		
		return &n->key;
	}

	//++prefix
	Iterator& operator++(){																	

		while(idx < sz)  //idx < table_size
		{
			if(n->next)  //if there is a next elem in the list/chain
			{
				n = n->next;  //go to next elem
				return *this;
			}
			else
				++idx;  //skip if not (NULL)

			if(idx == sz)  //if idx is at end
			{
				n = nullptr;  //return NULL
				return *this;
			}
			auto mv = ptr->table[idx].head;  //head of the current list at index = idx
			if(mv)  //if head != NULL
			{
				n = mv;
				return *this;
			}
		}
		return *this;
	}

	//++suffix
	Iterator operator++(int){																

		Iterator it{*this};
		operator++();
	
		return it;
	}

	//operator==
	friend bool operator==(const Iterator& lhs, const Iterator& rhs){											

		return lhs.n == rhs.n;
	}

	//operator!=
	friend bool operator!=(const Iterator& lhs, const Iterator& rhs){											

		return !(lhs.n == rhs.n);
	}
};

#endif // ADS_SET_H
