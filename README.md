# Route planner
## Description 
A console application has been implemented to search for routes from any city to any city, allowing no more than one transfer between different types of transport. HTTP requests are used for external data interaction through the cpr library, and JSON responses are processed using the **nlohmann/json** library. Caching is supported

## Features
- Searching for possible routes on a specified date (passed as a command-line argument).
- Integration with **Yandex Schedules API**.
- Support for routes with up to one transfer (e.g., plane + train).
- Caching of intermediate results in memory and on the file system to minimize API requests.
- Automatic downloading of external libraries via FetchContent (no need to manually install libraries).

## How to use

### Compile the project with CMake  
```cpp
- mkdir build && cd build 
- cmake ..
- cmake --build .
```

### Run the program

Use flags:</br>
```--from``` 'city' </br>
```--to``` 'city'</br>
```--date``` 'YYYY-MM-DD'</br>
```--recommend``` (optional) — if you want the program to recommend the optimal route. You will be asked how many hours you want to spend in a transfer city.</br>

### Example of the request
```cpp
./wayhome --from pskov --to moscow --date 2025-04-02
```