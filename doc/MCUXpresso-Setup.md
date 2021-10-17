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


# Opening the Program and Editing Source Code:
1. Open the Program
2. Create a workspace in your Documents Folder or wherever you want to do your work. This isn't the Project Repository, but rather a location for reference for the program. MCUXpresso allows you to open a workspace where you can have multiple projects that share the same settings, hence it's better to have it separate from our source code. I use the path `"/Users/davin/ubcRocket/wb_avionics/firmware"` that I made for my workspace and Rocket work.

<img width="611" alt="image" src="https://user-images.githubusercontent.com/24193569/136500769-1f268bf3-a5f0-4eb2-acee-45bc02e775d4.png">


3. After closing the welcome screen, "Import a Project from file system." You can copy the project into the workspace but that's optional.
4. You will know it's correctly working if you can see all of our Whistler Blackcomb source files in the Project Explorer.

<img width="611" alt="image" src="https://user-images.githubusercontent.com/24193569/136500504-34572f9d-e98c-4dd1-9303-1a3c53e5976d.png">

Once you have it working you can use your terminal to use Git and follow our normal methods for version control.


# Building Source:

Ensure that any changes you make to files shared by x86 and our Board compile without errors by doing a Build in MCUXpresso. This compiles for the Board enviroment instead of the x86 environment we build for in VSCode.

<img width="722" alt="image" src="https://user-images.githubusercontent.com/24193569/137638808-2ae35c65-1d96-437e-aa6a-dd7e8e4c698b.png">

