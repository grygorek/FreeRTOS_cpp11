# Porting to C++2a

[] Write tests for std::jthread  
[] Write tests for std::stop_token  
[] Write tests for std::stop_source  
[] Write tests for std::stop_callback  
[] Write tests for std::counting_semaphore - not supported in 10.2
[] Write tests for std::binary_semaphore   - not supported in 10.2
[] Write tests for std::latch              - not supported in 10.2
[] Write tests for std::barrier            - not supported in 10.2
[] Research other possible stl features that must be implemented  

[x] Find a platform running on qemu with sufficient resources to run all the tests  
   (note: qemu and FreeRTOS must support the same platform)  
[] update FreeRTOS source
[] github actions: 
  * test c++14, c++17, c++20, gcc8.2, gcc9.3, gcc10.2
  * run tests in qemu  
  
[x] review [issue](https://github.com/grygorek/FreeRTOS_cpp11/issues/18)