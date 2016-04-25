## UEFI使用OpenSSL

edk2的CryptoPkg里面会使用OpenSSL进行基础密码学计算，但是之后的使用我有点混乱了。

和《UEFI原理与编程》的做法一样，写一个inf文件，在里面用我的C程序。我的程序开始想使用CryptoPkg里面那个密码学的Protocol，但是运行的时候LocateProtocol失败了，之后我就想到，Protocol一般是给硬件对象用的，不用Protocol也是能用OpenSSL库的。于是我放弃了Protocol，用普通的方式用OpenSSL，只在inf文件里面用CryptoPkg，这样头文件的包含不会有问题。

接下来build EmulatorPkg，然而最后链接失败。查了下edk2的各种spec，我觉得应该在我的inf文件里面的``[LibraryClasses]``里面加上OpensslLib，然而接下来build的时候直接告诉我没有OpensslLib.再查了下dsc文件的spec，明白了应该在EmulatorPkg.dsc里面的``[LibraryClasses]``里面加上``OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf``，这样它使用的inf文件里面的OpensslLib便能被识别了，最后编译链接成功。

接着把构建出来的efi文件放上模拟器的磁盘，OVMF中运行，它正确地算出了一个文件的哈希值，成功！
