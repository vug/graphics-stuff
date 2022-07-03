Currently I'm at parental leave. I had started working as a graphics engineer professionally last year, and worked for a solid 9 months. :-) And more than half of that time I started writing C++ for work. So, I'm at the beginning of my graphics programming journey. Slowly grasping concepts and design patterns common to rendering and why and how C++ is used commonly in this domain.

When writing software one usually borrows certain functionalities from external libraries, or from the operating system etc. And frequently occurring usages of functionalities, or, domain specific concepts that rely on those functionalities are wrapped into abstractions, i.e. to classes that correspond to a specific concept.

In real-time graphics programming the most common approach is to use a graphics library API that utilizes the GPU. These APIs, [OpenGL](https://www.opengl.org/), [DirectX](https://en.wikipedia.org/wiki/DirectX), Vulkan being the main ones, provide commands that use the graphics card drivers using which one feeds the necessary data to the GPU and run the draw call commands on them. Please see Section [1.2 What is the OpenGL Graphics System?](https://www.khronos.org/registry/OpenGL/specs/gl/glspec46.core.pdf) of "The OpenGL® v4.6 Core Profile Specifications" for a more in depth explanation of what a graphics API is.

As an example, they provide a variety of buffers (types of memory allocations) such as vertex array, vertex buffer and index buffer, using which we can implement a graphics concept called a Mesh, that describes a geometry in space. A vertex buffers hold the data about vertices that makes a geometric object such as vertex positions, surface normals, colors assigned to vertices etc., index buffer holds the information about how (usually triangular) faces are made out of those vertices. A Vertex Array combines vertex buffers and index buffers and it can store one or more meshes in it. (See [OpenGL - Vertex Specification](https://www.khronos.org/opengl/wiki/Vertex_Specification))

In general, these objects that are provided by the API for rendering, such as buffers, textures etc. can be called resources. APIs have functions to create a resource and the CPU side refers to them via handles. For example, in OpenGL the function to create a Vertex Array is [`glGenVertexArrays`,](https://docs.gl/gl4/glGenVertexArrays) and for buffer is [`glGenBuffers`](https://docs.gl/gl4/glGenBuffers)

In below functions, first parameter indicates how many of these objects we are going to create, and second parameter is a pointer to the id/handle of the first of them.

```cpp
int vao, vbo, ebo;
glGenVertexArrays(1, &vao);
glGenBuffers(1, &vbo);
glGenBuffers(1, &ebo);
```

In OpenGL the handles are simple integers (more specifically typedef'd `unsigned int` as `GLuint`. In theory, in a different implementation of OpenGL `GLuint` might correspond to a different data structure.) In Vulkan they are more obscure types that wrap some pointers.

Once the resource is acquired using the generation function we can call further functions on them to use them. For example, [`glBindBuffer`](https://docs.gl/gl4/glBufferData) is to select a buffer, and [`glBufferData`](https://docs.gl/gl4/glBufferData) can be used to allocate memory on GPU and upload data from CPU memory to GPU into selected buffer.

```cpp
std::vector<MyVertex> vertices = { ... }
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertex) * vertices.size() , vertices.data(), GL_STATIC_DRAW);
```

Above code assumes that the vertex data is stored in a `vector` of `MyVertex` structs, then selects the created vertex buffer using its handle, then uploads the data from the vector in CPU memory to GPU memory.

These type of operations happen a lot in a rendering system, it's better to abstract away the direct OpenGL calls behind a Mesh class.

There is a programming pattern called [RAII](https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization), which stands for "Resource Acquisition Is Initialization". It is the approach for acquiring the resources at object creation and deleting them at object destruction automatically, so that, the users of the abstraction does not have to think about them. The existence and availability of the resource throughout the lifetime of the object is a "class invariant". In C++ class invariants are set in constructor functions. When the execution leave a scope the objects allocated on its stack are destroyed by calling their destructor functions. That where the resources should be deallocated/freed.

In our case, we want our handles for Graphics API objects to be created automatically, and to be destroyed automatically. Users should not even be aware of those handles for production use. (for debugging accessing them is useful).

```cpp
class Mesh
{
  Mesh()
  {
    // acquire arrays/buffers here and store their handles
  }

  ~Mesh()
  {
    // delete them using their handles here
  }

  // methods to use a mesh (bind, draw etc)
  void bind();
  void unbind();
  void uploadData(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
}
```

To study this for a more general case, let's forget about the specific graphics API (OpenGL) and specific graphics object (Mesh) and specific resources (buffers and array) for the moment. Instead have a generic, mock resource.

```cpp
// ResourceHandler.h
#pragma once

#include <unordered_set>

class ResourceHandler
{
public:
  static int createResource(); // [1]
  static void useResource(int handle); // [2]
  static void deleteResource(int handle); // [3]

private:
  static int counter; // [1, 5]
  static std::unordered_set<int> availableResources; // [4, 5]
};
```

1. Creating a resource will return its handle. Handles are unique integers. Uniqueness comes from by increasing a counter.
2. For using a resource we provide the handle of that resource.
    * We can provide a handle for an existing resource, and simply use it
    * Or accidentally we can provide a non-existing handle. Either a handle that is not created yet, or a handle that belongs to a deleted object. For these cases, the outcome depends on the specific API, in our abstraction let's throw an exception to simulate accessing a non-allocated memory location.
3. Resources can be deleted / freed too when not needed anymore. (Say we unload / deleted a mesh in an Editor)
4. An `unordered_set` of handles, aka `availableResources` tracks currently existing handles.
5. Also everything is static. (didn't bother to use a singleton pattern for simplicity)

A potential implementation of this mock resource could be as below:

```cpp
// ResourceHandler.cpp
#include <cassert>
#include <iostream>
#include <set>
#include <string>

int ResourceHandler::counter{};
std::unordered_set<int> ResourceHandler::availableResources{};

int ResourceHandler::createResource()
{
  auto [valueIterator, didInsert] = availableResources.insert(++counter);
  assert(didInsert); // since we increment counter at each call ids don't repeat and we should be able to insert
  std::cout << "Acquired Resource[" << *valueIterator << "] from ResourceHandler\n"; // [1]
  return *valueIterator;
}

void ResourceHandler::useResource(int handle)
{
  if (!availableResources.contains(handle))
    throw std::exception{("[Exception] Attempting to use non-existing Resource[" + std::to_string(handle) + "]").c_str()}; // [2]
  std::cout << "Used Resource[" << handle << "]\n"; // [1]
}

void ResourceHandler::deleteResource(int handle)
{
  auto search = availableResources.find(handle);
  if (search == availableResources.end())
  {
    std::cerr << "[Exception] Attempting to delete non-existing Resource[" << std::to_string(handle) << "]\n"; // [2, 3]
    std::terminate();
  }
  availableResources.erase(search);
  std::cout << "Deleted Resource[" << handle << "] via ResourceHandler\n";
}
```

1. Each function prints the handle id and the operation done to stdout for successful operations for studying/debugging purposes
2. Failed operations throw exceptions.
    * I should have just print things to stderr instead of throwing exceptions, which compilated the study. But chose to use them anyway for extra fun. :-)
3. `deleteResource` will be called from destructors. And one should simply not throw an exception from a destructor. Because if this happens while "stack unwinding" (when execution ends a scope and stack variables are destructed in reverse order) the exception mechanism stops the unwinding process and just terminates. There is no one to catch the exception. Therefore, using a print to `cerr` in this case to make my life easier.

We'll also use following macros to simplify the study/debugging

```cpp
// macros.h
#pragma once

#include <iostream>

#define PRINT_EXPR(expression)        \ // [1]
  printf("[expr] %s\n", #expression); \
  expression;

#define EXCEPTION(expression)           \ // [2]
  try                                   \
  {                                     \
    printf("[expr] %s\n", #expression); \
    expression;                         \
  }                                     \
  catch (std::exception & e)            \
  {                                     \
    std::cerr << e.what() << "\n";      \
  }
```

1. `PRINT_EXPR` is a simple trick to print the C++ expression to stdout first, then run that expression.
2. `EXCEPTION` is another trick to again print the C++ expression that is known to throw an exception, then run the expression, catch the exception and print the error message.
    * Not that, the expression is now placed inside a try block. Therefor this cannot be used to create variables in outer scope.

Below is a test of our mock resource system.

```cpp
// TestResourceHandler.cpp
#include "ResourceHandler.h"
#include "macros.h"

int main()
{
  PRINT_EXPR(int handle1 = ResourceHandler::createResource()); // [1]
  PRINT_EXPR(int handle2 = ResourceHandler::createResource());
  PRINT_EXPR(ResourceHandler::useResource(handle1)); // [2]
  EXCEPTION(ResourceHandler::useResource(3)); // [3]
  EXCEPTION(ResourceHandler::deleteResource(3)); // [4]
  PRINT_EXPR(ResourceHandler::useResource(handle2)); // [5]
  PRINT_EXPR(ResourceHandler::deleteResource(handle1)); // [6]
  EXCEPTION(ResourceHandler::useResource(handle1)); // [7]
  EXCEPTION(ResourceHandler::deleteResource(handle1)); // [8]
}
```

1. Create two resources
2. Use the first one
3. attempt to use a non-existing resource, causing an exception
4. attempt to delete a non-existing resource, causing an exception
5. use the  second resource
6. delete the first resource
7. attempt to use deleted first resource, causing an exception
8. attempt to use delete already deleted first resource, causing an exception

Here is the output:

```txt
[expr] int handle1 = ResourceHandler::createResource()
Acquired Resource[1] from ResourceHandler
[expr] int handle2 = ResourceHandler::createResource()
Acquired Resource[2] from ResourceHandler
[expr] ResourceHandler::useResource(handle1)
Used Resource[1]
[expr] ResourceHandler::useResource(5)
[Exception] Attempting to use non-existing Resource[5]
[expr] ResourceHandler::deleteResource(5)
[Exception] Attempting to delete non-existing Resource[5]
[expr] ResourceHandler::useResource(handle2)
Used Resource[2]
[expr] ResourceHandler::deleteResource(handle1)
Deleted Resource[1] via ResourceHandler
[expr] ResourceHandler::useResource(handle1)
[Exception] Attempting to use non-existing Resource[1]
[expr] ResourceHandler::deleteResource(handle1)
[Exception] Attempting to delete non-existing Resource[1]
```

Our mock functions properly. Now let's write some wrappers over this resource.

First wrapper is a simple, obvious one:

```cpp
// Wrapper01.h
#pragma once // [1]
#include <string>

class Wrapper
{
public:
  int handle = -1; // [2]
  std::string state; // [3]

  Wrapper(const std::string &state); // [4]
  ~Wrapper(); // [5]

  void use() const; // [6]
};
```

1. For the rest of this text I'm going to ignore `pragma`, `include` etc statements for brevity.
2. -1 means no resources acquired
3. this string represents/simulates a state. State of a rendering object can be a name, if it's a mesh a vector of vertices and another vector of indices etc.
4. One constructor that takes a state and acquire resource
5. destructor deallocated resource
6. One method to represent various usages of an object (bind, unbind, upload data etc)

It's implementation is as follows:

```cpp
// Wrapper01.cpp
Wrapper::Wrapper(const std::string &state)
    : handle(ResourceHandler::createResource()), state(state) {}

Wrapper::~Wrapper()
{
  ResourceHandler::deleteResource(handle);
}

void Wrapper::use() const
{
  ResourceHandler::useResource(handle);
}
```

Looks nice and clean. But when used in real-life a surprise is excepting us:

```cpp
// TestWrapper01.cpp
int main()
{
  PRINT_EXPR(Wrapper w1 = Wrapper("MyMesh")); // [1]
  PRINT_EXPR(w1.use());
  {
    PRINT_EXPR(Wrapper w2 = w1); // [2]
    PRINT_EXPR(w2.use());
    std::cout << "--End of inner scope--\n"; // [3]
  }
  EXCEPTION(w1.use()); // [4]
  std::cout << "--End of main scope--\n"; // [5]
}
```

1. Created an object and used it
2. In an inner scope created a new object as a copy of the first one. This could simple be a function call that takes a Wrapper, `func(Wrapper w)`;
3. at the end of its scope the destructor for `w2` will be called, which'll destroy its handle
4. Since `w1` and `w2` were using the same handle, `w1.handle` is still `1` though it's a deleted resource now. Attempt to use causes an exception!
5. Similarly, even if we hadn't use it again, while getting destructed at the end of the main scope, `w1` would throw an exception, because it'll attemp to delete the now deleted resource.

Here is the output of this test

```txt
[expr] Wrapper w1 = Wrapper("MyMesh")
Acquired Resource[1] from ResourceHandler
[expr] w1.use()
Used Resource[1]
[expr] Wrapper w2 = w1
[expr] w2.use()
Used Resource[1]
--End of inner scope--
Deleted Resource[1] via ResourceHandler
[expr] w1.use()
[Exception] Attempting to use non-existing Resource[1]
--End of main scope--
[Exception] Attempting to delete non-existing Resource[1]
```

Now, I'm going to introduce a methodology that I've seen on many StackOverflow posts to make the copy and move constructor/assignments explicit. It's a great way to study these operations in C++. I wish there were some debugging mechanism embedded in compilers / IDEs that can be triggered when constructor/destructers are called.

Add following declarations for copy/move construction/assignment:

```cpp
// Wrapper02.h
  Wrapper(const Wrapper &other); // [1]
  Wrapper &operator=(const Wrapper &other); // [2]
  Wrapper(Wrapper &&other); // [3]
  Wrapper &operator=(Wrapper &&other); // [4]
```

1. A copy constructor
2. A copy assignment
3. A move constructor
4. A move assignment

And implement them as follows:

```cpp
// Wrapper02.cpp
Wrapper::Wrapper(const Wrapper &other)
    : handle(other.handle), state(other.state) // [1]
{
  printf("Copy constructing... (handle, state) dst: (%d, %s), src: (%d, %s)\n", // [2]
    handle, state.c_str(), other.handle, other.state.c_str());
}

Wrapper &Wrapper::operator=(const Wrapper &other)
{
  printf("Copy assigning... (handle, state). dst: (%d, %s), src: (%d, %s)\n",  // [2]
    handle, state.c_str(), other.handle, other.state.c_str());
  handle = other.handle; // [3]
  state = other.state;
  return *this;
}

Wrapper::Wrapper(Wrapper &&other)
    : handle(std::move(other.handle)), state(std::move(other.state)) // [4]
{
  printf("Move constructing... (handle, state) dst: (%d, %s), src: (%d, %s)\n", // [2]
    handle, state.c_str(), other.handle, other.state.c_str());
}

Wrapper &Wrapper::operator=(Wrapper &&other)
{
  handle = std::move(other.handle); // [5]
  state = std::move(other.state);
  printf("Move assigning... (handle, state). dst: (%d, %s), src: (%d, %s)\n", // [2]
    handle, state.c_str(), other.handle, other.state.c_str());
  return *this;
}
```

Here we implemented these operations to be the same as the ones that would be generated by the compiler. Copy constructor/assignment copies each member, move constructor/assignment moves each member. After copying members will have the same values. After move, moved-in members will have the former values of moved-out members, and moved-out members will be "emptied out".

1. This copy constructor takes the object to be copied (src) as a const reference, copies each member into object to be constructor member at initialization.
    * As can be seen, this directly copies the original handle. After construction there are two objects with the same handle. And they are not in sync, i.e. there is no mechanism that makes it sure that if one is destructed, and their handle is deleted, to force the other one to be destructed too.
    * If one of the copies destruced, the handle of the other one will be dangling.
2. A print statement displays the values of dst and src handles and states. This is the neat trick to detect copy and move operations and what's going on in them.
3. Copy assignment is similar to copy construction but individual members are copied from the other, instead of initialized at construction.
4. Move construction is similar to copy construction except each member is moved at initialization not copied.
    * `state(std::move(other.state))` makes the new `state` to point at the memory location of the `char*` make `other.state` a `nullptr`. So, actually, nothing is moved in the memory, `move` does not move things around. :-)
    * a "moved-out" object is expected/assumed not to be used after the move operation.
5. Move assignment does the moves from src to dst object members instead of doing to moves at initialization.

Let's run our test app and see the outputs

```cpp
// TestWrapper02.cpp
Wrapper makeWrapper() // [1]
{
  PRINT_EXPR(Wrapper w = {"custom-made"});
  PRINT_EXPR(w.state += " Wrapper");
  PRINT_EXPR(return w);
}

int main()
{
  PRINT_EXPR(Wrapper w1 = Wrapper("MyMesh"));
  PRINT_EXPR(w1.use());
  {
    std::cout << "--Begin inner scope--\n";
    PRINT_EXPR(Wrapper w2 = w1); // [2]
    PRINT_EXPR(w2.use());
    std::cout << "--End inner scope--\n"; // [3]
  }
  EXCEPTION(w1.use()); // [4]

  PRINT_EXPR(Wrapper w3 = makeWrapper()); // [5]
  EXCEPTION(w3.use()); // [6]

  std::cout << "--End main scope--\n"; // [7]
}
```

1. Introducing a helper function to trigger the move construction. When `w` is returned, its content will be moved
2. Thanks to our custom copy construction, will see the copy explicitly
3. At the end of the inner-scope the copy `w2` will be destructed, deleting its handle, which is the same handle has `w1`'s handle.
4. Now we've seen why using `w1` causes an exception
5. Let's do a move operation
6. Because the moved out `w` of `makeWrapper` is destructed, it deletes the resource of its handle, which is the handle of `w3`, hence this triggers an exception too.
7. when the main scope ends it'll destruct `w3` and `w1` both of their resources are already deleted, causing further exceptions.
    * first exception due to deletion of `w3` will happen inside the destructor of `w3`. When destruction exception happens in this situation of "stack unwinding" it'll terminate the program.

output below matches with our understanding from above.

```txt
[expr] Wrapper w1 = Wrapper("MyMesh")
Acquired Resource[1] from ResourceHandler
[expr] w1.use()
Used Resource[1]
--Begin inner scope--
[expr] Wrapper w2 = w1
Copy constructing... (handle, state) dst: (1, MyMesh), src: (1, MyMesh)
[expr] w2.use()
Used Resource[1]
--End inner scope--
Deleted Resource[1] via ResourceHandler
[expr] w1.use()
[Exception] Attempting to use non-existing Resource[1]
[expr] Wrapper w3 = makeWrapper()
[expr] Wrapper w = {"custom-made"}
Acquired Resource[2] from ResourceHandler
[expr] w.state += " Wrapper"
[expr] return w
Move constructing... (handle, state) dst: (2, custom-made Wrapper), src: (2, )
Deleted Resource[2] via ResourceHandler
[expr] w3.use()
[Exception] Attempting to use non-existing Resource[2]
--End main scope--
[Exception] Attempting to delete non-existing Resource[2] (might terminate IRL)
[Exception] Attempting to delete non-existing Resource[1] (might terminate IRL)
```

What can we do about this?

2) Options: A) implement copy constructor with creating a new handle B) Delete copy constructor, implement move constructor.

3) say this is similar to `unique_ptr`.

4) say, better to use `shared_ptr`, so we don't have to write the move constructor ourselves. downside is to allocate on heap, which is not that bad.

4b) also, this is a simple theoretical study. In real-life, usually it's not the best to allocate one resource per object, we usually bundle multiple objects into single resource. For example, allocate a big vertex buffer and fill it with mesh data of multiple meshes, or, have texture atlases that contain multiple textures etc.

Also, discuss factor functions/methods. We don't want to inherit from `Mesh` to be able to generate specific geometries such as `Quad`, `Cube`, `UVSphere`, `IcoSphere`...

Discuss named and unnamed return value optimization in factory methods. Factory method that returns `shared_ptr` instead.

---

* next: delete copy constructor
* next: `Mesh& mesh` as input to manipulate it
* next: creating/allocating objects on stack vs. on heap (aka dynamic objects)
* next: without copy-constructor our class looks like a `unique_ptr`. We could have tracked the number living copies of the object, which'd make it look like a `shared_ptr`. Instead of reinventing the wheel, just use them.
* next: this Mesh abstraction by itself is not enough. It can be good for simple applications where each mesh is hard-coded etc., for a generative art project. We let the main scope to own the mesh. However, in a more realistic project, we want to have another abstraction to manage all of our assets including Mesh, such as Textures, Materials etc. We should be able to load and unload a mesh, put it into a scene or remove from a scene, have means of accessing it later etc. (say for mouse picking).
  * can have a `std::unordered_map<std::string, Mesh> where string is the "name"/"id" of the Mesh in our asset manager. Or even a simple mechanism could be to have a vector of assets.

```cpp
struct Assets 
{ 
  std::vector<std::shared_ptr<Mesh>> meshes;
  std::vector<std::shared_ptr<Texture>> textures;
  // etc
};
```

Of course, better to have further methods to manage these assets (load, delete, access by id, search). But that's a topic for another day.
