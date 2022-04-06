# Plugin: Modular Features Extra Actions

![image](https://user-images.githubusercontent.com/77353979/161829306-28db152a-0ddf-4c8a-a23c-3c3cb8e8dcdb.png)

## The plugin:

Inspired by some features from UE5's Valley of the Ancient sample, this plugin integrates GAS and Enhanced Input to the existing Game Features and Modular plugins from Unreal Engine, extending some Feature Actions to:

1. Add Attribute;
2. Add Gameplay Abilities;
3. Add Gameplay Effects;
4. Add Enhanced Input Mappings;
5. Spawn Actors;

**Example Project:**
There's a project with already built functions using multiplayer that i created using this plugin: [Project Elementus](https://github.com/lucoiso/UEProject_Elementus)

**This project is being built on Unreal Engine 5**

Post on Unreal Engine Forum: https://forums.unrealengine.com/t/free-modularfeatures-extraactions-plugin-modular-gas-enhanced-input-and-more/495400

# Installation
Just clone this repository or download the .zip file in the latest version and drop the files inside the 'Plugins/' folder of your project folder.
Note that if your project doesn't have a 'Plugins' folder, you can create one.

# Documentation:

## Initial Setup - Ability Binding:
To bind a ability input, implement the **AbilityInputBinding (C++: IAbilityInputBinding)** interface inside your **pawn or controller class** and implement the **SetupAbilityInputBinding** and **RemoveAbilityInputBinding** that comes with the AbilityInputBinding interface. The plugin will call these functions to bind the abilities.  
_Note that functions that come by interfaces doesn't have a body, you will need to create them._  

## Initial Setup - Ability Binding:
Classes that can receive the framework need to send themselves as Game Framework Component Receiver and when they are ready to receive the framework, send the Game ActorReadyGame Framework Component Extension Event.

C++ Functions:

> Add Receiver:  
> ```UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);```
  
> Remove Receiver:  
> ```UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);```
    
> Send Extension Event:  
> ```UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);```
    
## GFA Extra Actions: Add Abilities

![image](https://user-images.githubusercontent.com/77353979/161823255-1da59111-2fd3-4ada-a060-3194ff6767a6.png)  
_Screenshot from [Project Elementus](https://github.com/lucoiso/UEProject_Elementus)_

1. Target Pawn Class: Pawn class that will receive the abilities.
2. Require Tags: Required tags that this actor should own to apply the game framework.
3. Input Binding Owner: Class that implemented the AbilityInputBinding interface.
4. InputID Enumeration Class: Enumeration class to use as inputID provider to bind the abilities. _Note that this plugin currently only supports Blueprint Enumerations._
5. Ability Mapping: Abilities to add and they binding settings.  
5.1. Ability Class: Gameplay Ability class to add.  
5.2. Input Action: Enhanced Input Action to bind to ability activation.  
5.3. Ability Level: Ability level to add to target ability system component.  
5.4. InputID Value Name: The same as Display Name in your InputID Enumeration Class.  

## GFA Extra Actions: Add Attribute

![image](https://user-images.githubusercontent.com/77353979/161825892-dc85b43a-1d4b-4d52-8726-4071a53fadcd.png)  
_Screenshot from [Project Elementus](https://github.com/lucoiso/UEProject_Elementus)_

1. Target Pawn Class: Pawn class that will receive the abilities.
2. Require Tags: Required tags that this actor should own to apply the game framework.
3. Attribute: AttributeSet class to add.
4. Initialization Data: DataTable containing Attributes MetaData to initialize attributes.

## GFA Extra Actions: Add Effects

![image](https://user-images.githubusercontent.com/77353979/161826293-7d956769-10cf-4505-bcf4-462564100a98.png)
_Screenshot from [Project Elementus](https://github.com/lucoiso/UEProject_Elementus)_

1. Target Pawn Class: Pawn class that will receive the abilities.
2. Require Tags: Required tags that this actor should own to apply the game framework.
3. Effects Mapping: Gameplay Effects to add and they settings:  
3.1. Effect Class: Gameplay Effect class to add to target.  
3.2. Effect Level: Gameplay Effect Level that will be passed to target ability system component.    
3.3. Set by Caller Params: Set by Caller parameters to pass to this gameplay effect.  
3.3.1. Key (Gameplay Tag): Set By Caller tag.  
3.3.1. Value (Float): Set By Caller float value.  

## GFA Extra Actions: Add Enhanced Input Mapping

![image](https://user-images.githubusercontent.com/77353979/161827205-d2e3121e-8eed-43ab-8310-a83753b34a05.png)  
_Screenshot from [Project Elementus](https://github.com/lucoiso/UEProject_Elementus)_

1. Target Pawn Class: Pawn class that will receive the abilities.
2. Require Tags: Required tags that this actor should own to apply the game framework.
3. Input Binding Owner: Class that will own this input binding and/or implemented the AbilityInputBinding interface.
4. Input Mapping Context: Enhanced Input Mapping to add to target.
5. Mapping Priority: Enhanced Input Mapping priority.
6. Action Bindings: Enhanced Input Actions that will be binded to target and they settings.  
6.1. Action Input: Enhanced Input Action to bind.    
6.2. Ability Binding Data: Ability binding informations that could be passed if the user wants to bind this action input to a ability activation without pass a Gameplay Ability.  
6.2.1. Setup Ability Input: If true, will bind this Input Action with SetupAbilityInputBinding via AbilityInputBinding interface.  
6.2.2. InputID Enumeration Class: Enumeration class to use as inputID provider to bind the abilities. _Note that this plugin currently only supports Blueprint Enumerations._  
6.2.3. InputID Value Name: The same as Display Name in your InputID Enumeration Class.    
6.3. UFunction Bindings: Binding informations to pass with this Input Action, this is necessary to bind this action to existing functions.  
6.3.1. Function Name: UFunction name that this Input Action will bind.  
6.3.2. Triggers: Trigger array referred to binding management.  

## GFA Extra Actions: Spawn Actors

![image](https://user-images.githubusercontent.com/77353979/161828287-f480dba0-79a2-485a-8643-ff909d76a7e5.png)
_Screenshot from [Project Elementus](https://github.com/lucoiso/UEProject_Elementus)_

1. Target Level: Map where this actor will be spawned.
2. Spawn Settings: Array containing actors spawning settings.  
2.1. Actor Class: Actor class to spawn.  
2.2. Spawn transform: Initial transform that will be applied to the spawned actor.  
