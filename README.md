# QUnreal

### Quake (Trenchbroom) Map and WAD importer for Unreal 5.

<img src="https://github.com/chunky-cat/QUnreal/blob/main/.media/logo.png?raw=true" alt="qunreal_logo" width="60%" height=60%/>

## Installation:

* Create a folder named `Plugins` in your project if you don't already have one.
* clone the repository into the `Plugins` folder via git:

```bash
git clone https://github.com/chunky-cat/QUnreal && git submodule update --init
```
* Activate the QUnreal Plugin via editor or your project config file.

## Documentation:

[Read the Wiki](https://github.com/chunky-cat/QUnreal/wiki)

## features:

* import Quake Maps and WADs by simply dragging them into your content drawer.
* map Unreal Blueprints or C++ Classes to trenchbroom entities.
* use map files as static mesh libraries.

## TODO:

* import brushes with clip texture as colliders
* more options for setting the entity pivot
* add "reload" button into the map asset context menu
* support reimporting WAD files
* option to import textures from WAD to a separate folder for easier access
