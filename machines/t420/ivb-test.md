## T420使用Ivy Bridge CPU的测试

事实证明，T420是可以使用Ivy Bridge的CPU的，我已经在coreboot的T420移植的[wiki页面](https://www.coreboot.org/Board:lenovo/t420)写了相关的注意事项，总结下来有几点：

1. Sandy Bridge和Ivy Bridge的内存初始化是一样的(至少在coreboot是这样),核显做原生图形初始化有差别，需要区别处理，而且到现在，T420在固件阶段的图形初始化还是有严重的bug[1]  
2. 用Ivy Bridge CPU的时候一定要修补微码，否则系统很容易挂，用Sandy/Ivy Bridge的微码都行  
3. 调试的时候要注意一些问题，我在下面讲一下

有时系统会出一些奇怪的bug，但是可能不知道原因是什么，这样就只能靠猜测了。比如上面所说的微码的问题，那是在系统kernel panic好几次之后，怀疑可能是因为没修补微码导致的，然后用上微码之后就解决了。后面调试的时候还遇到过一些问题，我谈一点经验：

* USB调试口由于速度的原因，从那里得到的调试输出很可能不是全部的输出，因为很有可能系统中还有很多调试信息在输出缓冲区中，然后突然断电。  
* 拆散热器，换CPU后，不要怕麻烦而不把散热器装好，因为过热会导致系统突然断电。我有好几次遇到了莫名其妙的断电现象，最后才发现可能是CPU过热造成的，再加上上面所说的USB调试口不能完整输出数据的原因，我根本找不到问题出在哪里。此外，CPU过热可能也会导致初始化内存的过程中出错，原因不明。

现在用上i7-3720QM(QS版)的T420工作还不错，不过T420的散热还真压不住这颗CPU，可以看看我用boinc烤机的时候i7z的输出。[2]

> ```
> Socket [0] - [physical cores=4, logical cores=8, max online cores ever=4]
> TURBO ENABLED on 4 Cores, Hyper Threading ON
> Max Frequency without considering Turbo 2690.65 MHz (99.65 x [27])
> Max TURBO Multiplier (if Enabled) with 1/2/3/4 Cores is  36x/35x/34x/34x
> Real Current Frequency 2789.82 MHz [99.65 x 28.00] (Max of below)
> Core [core-id]  :Actual Freq (Mult.)      C0%   Halt(C1)%  C3 %   C6 %   C7 %  Temp      VCore
> Core 1 [0]:       2789.82 (28.00x)      99.8       0       0       0       0    90      1.0157
> Core 2 [2]:       2789.82 (28.00x)      99.8       0       0       0       0    94      1.0157
> Core 3 [4]:       2788.27 (27.98x)      88.4       0       1       0    11.3    91      1.0157
> Core 4 [6]:       2788.61 (27.98x)      88.4       0       0       0    10.1    90      1.0157
> ```

可以看出，CPU跑到2.8GHz的时候温度已经到90度了，用一个带功率计算功能的插排看了下功率，功率在70W左右，90W的电源还能用。如果我在负载不高的时候直接把boinc的CPU占用设为100%,系统还真的可以因为CPU温度突然升高而关机。所以感觉这机器最合适的是i7-3632QM这样低频一些的四核，修改ACPI也许也能避免系统突然关机的问题，不过降频应该还是避免不了的。

[1] 核显的问题已经由一个上游的commit解决

[2] UPDATE: 我应该改用一个双热管的散热器......

