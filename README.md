# PS5 WebKit Cache Remove

Removes the `/webkit/shell` folder for all users on a jailbroken PlayStation 5, which contains WebKit-related cache. Built using the latest version of [ps5-payload-sdk](https://github.com/ps5-payload-dev/sdk) and tested on my jailbroken 4.51 PS5 using [UMTX2](https://github.com/idlesauce/umtx2).

To send the payload to your PlayStation 5, you can use NetCat with the following command in a Linux terminal: `nc -v -q 0 IP PORT < PATH`.

Special thanks to [Storm21CH's PS5_Browser_appCache_remove project](https://github.com/Storm21CH/PS5_Browser_appCache_remove/) for providing the path to the WebKit cache. Unfortunately, the aforementioned project doesn't work with UMTX2, which is the main reason I created this project.
