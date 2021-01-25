### First thing to do
You need to clone some requisite addons to the addons folder of your OF:
- [ofxNbackTest](https://github.com/ryohajika/ofxNbackTest)
- [ofxSeekThermal](https://github.com/ryohajika/ofxSeekThermal)
- [ofxRHUtilities](https://github.com/ryohajika/ofxRHUtilities)

and the project requires ofxGui and ofxOpenCv, which is bundled to the OF package

### Open the project...
You should be good to go with the project, but you may need to reload addons using projectGenerator or modify the project settings to be able to compile the project on your PC (especially in the case you use macOS 11)

### Warning
The project is using `pthread`, which is a part of the unix-based system to manage the threading and is not available in windows. Which means this project is not ready for the windows-based PC yet.

