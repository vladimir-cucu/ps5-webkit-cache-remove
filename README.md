# PlayStation 5 WebKit Cache Remover

Removes the WebKit cache for all users on your jailbroken PlayStation 5 by deleting all `/user/home/<userid>/webkit/shell` directories from the PS5 file system. Built using the latest version of [ps5-payload-sdk](https://github.com/ps5-payload-dev/sdk) and tested on my 4.51 PS5 jailbroken using [UMTX2](https://github.com/idlesauce/umtx2).

To manually send the payload to your PS5, you can use NetCat with the following command: `nc -v -q 0 PS5_IP ELFLDR_PORT < PAYLOAD_PATH`.

Special thanks to [Storm21CH's PS5_Browser_appCache_remove project](https://github.com/Storm21CH/PS5_Browser_appCache_remove/) for providing the WebKit cache path. Unfortunately, that project doesn't work with UMTX2, which is the main reason I created this one.
