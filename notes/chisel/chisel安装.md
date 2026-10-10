chisel是scala语言的一个包，所以首先安装scala
推荐方法是使用下载器[coursier](https://get-coursier.io/docs/cli-installation)
```bash
# Download on x86-64 (aka AMD64)
curl -fL "https://github.com/coursier/launchers/raw/master/cs-x86_64-pc-linux.gz" | gzip -d > cs
# Download on ARM64
curl -fL "https://github.com/VirtusLab/coursier-m1/releases/latest/download/cs-aarch64-pc-linux.gz" | gzip -d > cs
# Then, run it with
chmod +x cs
./cs setup
```
运行coursier会下载安装scala和各种配套，包括以下这些：
![[Pasted image 20260831160023.png]]
没有魔法的情况下，安装可能会非常慢（二十分钟一个包），而且JVM会长时间保持0KB/s
可以先手动安装JAVA运行时[temurin](https://adoptium.net/installation/linux)，再运行cs
```bash
# Tools for installation
sudo apt install -y wget apt-transport-https gpg
# Download and verify
wget -qO - https://packages.adoptium.net/artifactory/api/gpg/key/public | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/adoptium.gpg > /dev/null
# Add apt source?
echo "deb https://packages.adoptium.net/artifactory/deb $(awk -F= '/^VERSION_CODENAME/{print$2}' /etc/os-release) main" | sudo tee /etc/apt/sources.list.d/adoptium.list
# Install with apt
sudo apt update
sudo apt install temurin-25-jdk
```
现在回到ysyx-workbench，初始化一个chisel示例项目，顺便运行试试
```bash
cd ~/ysyx-workbench/
bash init.sh npc-chisel
cd npc
make test
```
然后会看到报错缺少[mill](https://mill-build.org/mill/cli/installation-ide.html), which is项目构建和包管理的工具
mill推荐安装到项目目录下，使用./mill为每个项目运行对应版本
但npc的Makefile里使用的不是./mill而是mill
所以我还是选择全局安装：
```bash
 sudo curl -L https://repo1.maven.org/maven2/com/lihaoyi/mill-dist/1.1.8/mill-dist-1.1.8-mill.sh -o /usr/local/bin/mill
 sudo chmod +x /usr/local/bin/mill
```
最后再次运行`make test`确认安装正确，mill会下载所需的package，包括chisel
好结局：
![[Pasted image 20260831163822.png]]
（每次运行mill时可能会有几个WARNING，似乎是因为它使用了旧版不安全的java方法）