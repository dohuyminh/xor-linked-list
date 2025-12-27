# XOR: A property

## DISCLAIMER:
**EVERYTHING YOU ARE ABOUT TO READ ARE FOR EDUCATIONAL AND RECREATIONAL PURPOSES ONLY; ANY ATTEMPT ON APPLYING THIS ON PRODUCTION CODE MAY RESULT IN GETTING SCOLDED BY YOUR LOCAL SENIOR DEVELOPER/PROJECT MANAGER ON A CODE REVIEW.**

## 1. An introduction to XOR and the "trick":

In boolean algebra, an Exclusive Or (XOR/$\oplus$) is a binary operator that will return true if the 2 variables share different values, and false otherwise.

| $X$ | $Y$ | $X \oplus Y$ |
|---|---|-------|
| 0 | 0 |   0   |
| 0 | 1 |   1   |
| 1 | 0 |   1   |
| 1 | 1 |   0   |

An interesting property of this property arises when you apply XOR a second time to one of the variables:

$$
X \oplus Y \oplus Y \equiv X \\
X \oplus Y \oplus X \equiv Y
$$

So what does this have to do with a **Linked List**?

## 2. A brief introduction to doubly Linked List:

A doubly linked list is a recursive container data structure consisting of nodes, each holding a value and 2 references to its previous node and next node. In C/C++, one would typically implement the node data structure as follows:

```cpp
struct Node {
    int data;
    Node* prev;
    Node* next;
};
```

One might attribute why a doubly linked list is not so often used in production code to several reasons. The head's previous and tail's next both point to some "terminating" references (e.g. `nullptr` in C++, `null` in Java, `None` in Python). These are among the priority concerns:
- In memory, nodes are not typically placed contiguously, so it has no sense of temporal/spatial locality, thus cache misses are all over the place.
- As opposed to array, a doubly linked list does not support random access; one would have to potentially traverse the entire list to find an element. 
- While an element in an array of type ``T`` takes up ``sizeof(T)``, a node has a size of ``sizeof(T)`` AND extra 16 bytes (for the 2 pointers referencing the previous and next node; pointers are, after all, 8-byte objects after all). 

However, by interpreting a pointer as a 64-bit object, we can reduce the size of the node by 8 bytes with some bit magic and a lot of unreadable code.

## 3. XOR Linked List:

Given an address is a 8-byte (or 64-bit) object. We may redesign the node data structure as follows:

```cpp
struct Node {
    int data;
    uintptr_t xor;
};
```

where `xor = addr(prev(node)) ^ addr(next(node))`, which is a 8-byte object.

So the node now has no direct references to its previous and next node, and yet most of the Doubly Linked List operations can be implemented:

### 3.1. (Reverse) Traversing a linked list:

If we are given the address of the previous node, we can easily find out the next node as `addr(next) = addr(prev) ^ addr(next) ^ addr(prev) = curr.xor ^ addr(prev)`

Since the head's previous is `null`, we can continuously keep track of the `prev` and `curr` node pair to continuously figure out the next node as follows:

```
let addr(prev) <- null
let addr(curr) <- addr(head)

repeat until addr(curr) = null:
    do_stuff(curr.data)
    addr(next) <- curr.xor ^ addr(prev)
    addr(prev) <- addr(curr)
    addr(curr) <- addr(next)
```

A doubly linked list supports traversals on both directions, from head to tail and vice versa. I will leave this as an exercise for the reader.

### 3.1. Insertion of a node at a place:

Given 2 nodes next to each other `prev <-> curr`, now one might want to insert a new node in between `prev <-> new <-> curr`.

Originally in the linked list, we have:
- `prev.xor = addr(prev(prev)) ^ addr(curr)`
- `curr.xor = addr(prev) ^ addr(next(curr))`

Now we can create a node `new` with the provided data, and `new.xor <- addr(prev) ^ addr(curr)` to reflect the previous and the next node `new` is "pointing to". 

Now we need to make `new` `prev`'s next node and `curr`'s previous node, i.e.
- `prev.xor = addr(prev(prev)) ^ addr(new)`
- `curr.xor = addr(new) ^ addr(next(curr))`

To achieve this, we just need to perform couple XOR operations as follows:
- `prev.xor <- prev.xor ^ addr(curr) ^ addr(new)` (first XOR to make `prev.xor = addr(prev(prev))`, the second one to make `prev.xor = addr(prev(prev)) ^ addr(new)`)
- `curr.xor <- curr.xor ^ addr(prev) ^ addr(new)` (first XOR to make `curr.xor = addr(next(curr))`, the second one to make `curr.xor = addr(next(curr)) ^ addr(new)`)

This is essentially equivalent to steps to ensure the pointers are pointing correctly:

```
// traverse the list to find place to insert, between prev and curr

// equivalent to new <- new node(data, prev=prev, curr=curr)
new <- new node(data, xor=addr(prev) ^ addr(curr))

// equivalent to prev.next <- new
prev.xor <- prev.xor ^ addr(curr) ^ addr(new)

// equivalent to curr.prev <- new
curr.xor <- curr.xor ^ addr(prev) ^ addr(new)
```

### 3.2. Removal of a certain node:

Say now one doesn't want the node `new` anymore and wants to remove it; we need to revert it back to `prev <-> curr`. We can simply perform the deletion by applying the same operation again. 

From the state `prev.xor = addr(prev(prev)) ^ addr(new)`, when XORring again with `addr(new) ^ addr(curr)`, `prev.xor = addr(prev(prev)) ^ addr(curr)` (i.e. now prev.next = curr).

Same method applies for `curr`.

## 4. Why this probably doesn't get mentioned in your DSA course:

Because it's weird and confusing to most.

Most students learning about Data Structures in their university course probably learns about linked lists after array. The concept and implementation of a linked list is trippy as-is; imagine saying a bitwise operator can help make a data structure more space efficient.

It's also impractical.

There are some edge cases I have not mentioned discussing the operations' implementation. While those edge cases are covered in the demo code, it should be emphasized that in production code, cryptic algorithms like these are a nightmare to maintain (unless you are in the embedded programming space, and this is probably just Tuesday).

And in the end,...

It still suffers the flaws of a "vanilla" linked list: unfriendly for random access, an "element" still takes extra space for navigation and cache misses slow everything down.

This is a party trick, at best. 