# 软件功能

该程序会自动监听 Windows 环境下的复制操作，并将复制的内容中的换行符去除，然后将处理后的内容重新放回剪贴板。

* 按下`ctrl + c`时，会自动转换复制的内容

* 按下`ctrl + shift + alt + p`时，程序会暂停，再次按下时启用

# 快速开始

## 直接使用

1. 下载exe文件
2. 双击运行

## 从源码编译

* 下载代码
* 使用c++ 20标准编译运行

# 使用场景

在翻译论文时，如果重复时出现了换行的情况，则可能会出现翻译不顺，翻译不准的情况。使用该软件后则可以将要翻译的内容变成一句话，而不是多行一起翻译的情况。

## 例子

以`Deep Residual Learning for Image Recognition`论文的引言的第一段为例：

在chrome浏览器中打开一个pdf文件，然后复制其中的内容。

### 直接复制

```
Deeper neural networks are more difficult to train. We
present a residual learning framework to ease the training
of networks that are substantially deeper than those used
previously. We explicitly reformulate the layers as learning residual functions with reference to the layer inputs, instead of learning unreferenced functions. We provide comprehensive empirical evidence showing that these residual
networks are easier to optimize, and can gain accuracy from
considerably increased depth. On the ImageNet dataset we
evaluate residual nets with a depth of up to 152 layers—8×
deeper than VGG nets [41] but still having lower complexity. An ensemble of these residual nets achieves 3.57% error
on the ImageNet test set. This result won the 1st place on the
ILSVRC 2015 classification task. We also present analysis
on CIFAR-10 with 100 and 1000 layers.
```

其翻译结果为（使用deepl翻译）：

```
更深的神经网络更难训练。我们
我们提出了一个残差学习框架，以帮助训练
网络的训练。
的网络。我们明确地将各层重新表述为参照层输入学习残差函数，而不是学习无参照函数。我们提供了全面的经验证据，表明这些残差
网络更容易优化，并能通过大幅提高深度来获得准确性。
深度大大增加。在 ImageNet 数据集上，我们
对深度达 152 层的残差网络进行了评估--8 倍于 VGG 网络的深度。
比 VGG 网络更深[41]，但复杂度仍然更低。这些残差网络的集合在 ImageNet 测试集上实现了 3.57% 的误差。
在 ImageNet 测试集上取得了 3.57% 的错误率。这一结果赢得了
ILSVRC 2015 分类任务的第一名。我们还介绍了
100 层和 1000 层的 CIFAR-10 的分析。
```

### 去行以后翻译：

```
Deeper neural networks are more difficult to train. We  present a residual learning framework to ease the training  of networks that are substantially deeper than those used  previously. We explicitly reformulate the layers as learning residual functions with reference to the layer inputs, instead of learning unreferenced functions. We provide comprehensive empirical evidence showing that these residual  networks are easier to optimize, and can gain accuracy from  considerably increased depth. On the ImageNet dataset we  evaluate residual nets with a depth of up to 152 layers—8×  deeper than VGG nets [41] but still having lower complexity. An ensemble of these residual nets achieves 3.57% error  on the ImageNet test set. This result won the 1st place on the  ILSVRC 2015 classification task. We also present analysis  on CIFAR-10 with 100 and 1000 layers. 
```

其翻译结果为（使用deepl翻译）：

```
更深的神经网络更难训练。我们提出了一个残差学习框架，以简化比以前所用网络更深的网络的训练。我们明确地将各层重新表述为参照层输入学习残差函数，而不是学习无参照函数。我们提供了全面的经验证据，表明这些残差网络更易于优化，并能通过大幅提高深度获得准确性。在 ImageNet 数据集上，我们评估了深度高达 152 层的残差网络--比 VGG 网络[41]更深 8 倍，但复杂度仍然更低。这些残差网络的集合在 ImageNet 测试集上实现了 3.57% 的误差。这一结果赢得了 ILSVRC 2015 分类任务的第一名。我们还对 100 层和 1000 层的 CIFAR-10 进行了分析。
```



