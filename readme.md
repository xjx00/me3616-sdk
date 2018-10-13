ME3616 has 2 versions:
ME3616-G1A has GPS included.
ME3616-C1A has no GPS included.


for the ME3616-G1A version, you have to modify SDK\sdk\examples\flash\flash.c as
```
FLASH_TEST_BASE    0x00298000
```
and there is 128K（rom）+128K（ram）for the G1A version，
