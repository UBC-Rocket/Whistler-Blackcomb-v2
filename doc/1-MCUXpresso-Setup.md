## Setup
### Download Tools
1. MCUXpresso

Download from [here](https://www.nxp.com/design/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-integrated-development-environment-ide:MCUXpresso-IDE?tab=Design_Tools_Tab). You will need an account, sign up for one for free. All the default options should be fine. 

2. Chip SDK

Go to [here](https://mcuxpresso.nxp.com/download/78d26642c8a4ca1efecebe0f36145702) and download it. If that doesn't work for some reason you can also get the SDK package yourself [here](https://mcuxpresso.nxp.com/en/select), search for MK66FX1 and click on "Build MCUXpresso SDK. Select all and download SDK. Note that this might take a few minutes. 

![](images/SDKSelect.png)
![](images/SDKDownload.png)

After the SDK is installed open the Installed SDK window (if this isn't visible click Windows->Show View->Installed SDKs) on MCUXpresso and drag the SDK zip file. 

![](images/InstalledSDKs.png)

### Setup Project

Click File->Import, select Git->Projects from Git (with smart import) and select Clone URI. Enter appropriate information and project should import. Note that you can use console git (wsl, git bash, etc.) but importing the project to the workspace is a bit confusing so it's easier to use the UI. 
