## 3 - 05 - 26

### NEXT BIG THINGS TO DO:

- Make WindowManager manage event polling internally. `DONE`
- Creation of CameraManager `DONE`
- Make Scripts register themselves. `DONE`
- Make addScript recieve a function reference, not a string `DONE`
- Make elements scene aware instead of just being called in OnReloadMethod of SceneManager `DONE`
- Level Manager Refactor `DONE`
- Fix Level manager creation of tiles. `DONE`
- Avoid memory leak when creating a new cameraManager dynamically in SceneBuilder. Delete the instance when changing scene.`DONE`
- Remove all uses of GameCamera outside Camera Manager. `DONE`

- Think about having a FrameContext that will contain dt.
- Modifiy Tile Picker to update tile parallax in real time.
- Add a, doWhenChanged actuator in the UISlider.
- Create kernel architecture interface for system add-ons.
- Make Creation of TilePicker and Temrinal systems, not scripts.

- Remove pure static registration from registries.

- Change Particle Manager to not have singleton properties.
- Solve ownership issues between GameObject and GameCamera
- Fix references to managers inside snowlang. Pass them along trough I/O or another interface
- Remove ptrs use in Tile object inside Level Manger
- Refactor gameloader so you dont have to pass around functions from Mangers around internally.

- Error pipeline for the whole engine.
- Refactor Bullet Object. (or delete it completely)

- Refactor Dialogue Manager. Redistribute responsabilities across Dialogue Manager and GameText

- RenderizerParams should only accept CameraIDs, not GameCamera Objects.

- Inheritance from base id class to WindowID and CameraID
- Texture Key should not be in the constructor of Object Builder

- Change MapLifecycle to convert the Type automatically into unique_ptr<Type>

- Move all instances of oldRenderCommand into RenderCommand

- Remove all uses of Vector2f. Mudate towards custom-built Vector2f

- Create Base ID<Type> Object tpo use as ID<WindowInstance> ID<GameObject>

_PIORITY_

- Remove static gameObjectRegistry from GameObject class. `DONE`
- Make Scenes the ones that have the responsability of handling the lifetime of GameObjects. `DONE`

- Move on from QueuedManager inherited classes
- Replace all instances of QueuedManager with LifeCycleManager. Composition over inheritance.

- Make LevelManager part of Scene.

- Start moving towards a ECS model. _IN PROGRESS_
  - Sprite Component
  - CameraComponent
  - AnimationComponent
  - PhysicsComponent
  - ScriptComponent?
  - etc.

- Make WindowID part of the CameraComponent, not part of SpriteComponent
