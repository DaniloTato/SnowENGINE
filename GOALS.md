## 3 - 05 - 26

### NEXT BIG THINGS TO DO:

- Make WindowManager manage event polling internally. `DONE`
- Creation of CameraManager `DONE`
- Think about having a FrameContext that will contain dt.
- Make Scripts register themselves. `DONE`
- Modifiy Tile Picker to update tile parallax in real time.
- Add a, doWhenChanged actuator in the UISlider.
- Create kernel architecture interface for system add-ons.
- Make Creation of TilePicker and Temrinal systems, not scripts.

- Make addScript recieve a function reference, not a string `DONE`

- Remove pure static registration from registries.

- Make elements scene aware instead of just being called in OnReloadMethod of SceneManager `DONE`
- Change Particle Manager to not have singleton properties.
- Change Object builder declaratio of camera. Do not explicitely ask for the gameCamera, jsut the id.
- Avoid memory leak when creating a new cameraManager dynamically in SceneBuilder. Delete the instance when changing scene.
- Make Scenes the ones that have the responsability of handling the lifetime of GameObjects
- Make Scenes own the Managers
- Solve ownership issues between GameObject and GameCamera
- Convert Managers from singletons to members of Engine Object
- Fix references to managers inside snowlang. Pass them along trough I/O or another interface
- Level Manager Refactor
- Remove all uses of GameCamera outside Camera Manager.
- Refactor gameloader so you dont have to pass around functions from Mangers around internally.

- Error pipeline for the whole engine.
- Refactor Bullet Object. (or delete it completely)

- Fix Level manager creation of tiles. `DONE`
