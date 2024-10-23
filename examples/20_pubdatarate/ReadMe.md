## 20_pubdatarate Project Overview

The **20_pubdatarate** project demonstrates network communication data rates. The service generates a simple gray bitmap image based on user input and sends it to connected *Service Consumers*. Both the *Service Provider* and *Service Consumer* applications display the network data rate on the console, helping developers determine the optimal data rate for the application.

The project consists of three sub-projects:

1. **20_generated:** This is generated from the [LargeData.siml](./res/LargeData.siml) Service Interface document during CMake configuration or as a pre-build action in Visual Studio's `dummy` project.
2. **[20_pubservice](./pubservice/):** This sub-project provides a *Public Service* that generates and sends bitmap image data to connected consumers, based on user input.
3. **[20_pubclient](./pubclient/):** This sub-project includes a *Service Consumer* that receives and processes the image data from the Service Provider.

All communication is managed through `mcrouter`, with the AREG SDK ensuring automatic service discovery and fault tolerance, so the process startup order doesn’t affect functionality.
