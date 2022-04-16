# Plugin: Modular Features Extra Actions

![image](https://user-images.githubusercontent.com/77353979/163046114-e8f33b34-456c-47a2-942a-1cbffcfcf6b6.png)

## The plugin

This plugin integrates GAS and Enhanced Input to the existing Game Features and Modular plugins from Unreal Engine, extending the plugin to add some feature actions:

1. Add Attribute;
2. Add Gameplay Abilities;
3. Add Gameplay Effects;
4. Add Enhanced Input Mappings;
5. Spawn Actors;

## Links

**Marketplace:** [Modular Game Features - Extra Actions in Code Plugins - UE Marketplace](https://unrealengine.com/marketplace/en-US/product/modular-game-features-extra-actions)  
**Forum:** [[FREE] ModularFeatures_ExtraActions plugin - Modular GAS, Enhanced Input and more](https://forums.unrealengine.com/t/free-modularfeatures-extraactions-plugin-modular-gas-enhanced-input-and-more/495400)  
**Example Project:** [Project Elementus](https://github.com/lucoiso/UEProject_Elementus) - _Work in Progress but already using this plugin_  

# Installation
Just clone this repository or download the .zip file in the latest version and drop the files inside the 'Plugins/' folder of your project folder.
Note that if your project doesn't have a 'Plugins' folder, you can create one.

# Documentation

## Initial Setup - Ability Binding
To bind a ability input, implement the **AbilityInputBinding (C++: IAbilityInputBinding)** interface inside your **pawn or controller class** and implement the **SetupAbilityInputBinding** and **RemoveAbilityInputBinding** that comes with the AbilityInputBinding interface. The plugin will call these functions to bind the abilities.  
_Note that functions that come by interfaces doesn't have a body, you will need to create them._  

## Initial Setup - Add/Remove Framework Receiver
Classes that can receive the framework need to send themselves as Game Framework Component Receiver and when they are ready to receive the framework, send the Game ActorReadyGame Framework Component Extension Event.

C++ Functions

> Add Receiver:  
> ```UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);```
  
> Remove Receiver:  
> ```UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);```
    
> Send Extension Event:  
> ```UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);```
    
## MF Extra Actions: Add Abilities

![image](https://user-images.githubusercontent.com/77353979/163046339-31f39258-f81d-4c36-9c87-25667d6ec9c3.png)
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

## MF Extra Actions: Add Attribute

![image](https://user-images.githubusercontent.com/77353979/163046427-a5f182ff-312e-4f57-8f52-3baf889c1858.png)  
_Screenshot from [Project Elementus](https://github.com/lucoiso/UEProject_Elementus)_

1. Target Pawn Class: Pawn class that will receive the abilities.
2. Require Tags: Required tags that this actor should own to apply the game framework.
3. Attribute: AttributeSet class to add.
4. Initialization Data: DataTable containing Attributes MetaData to initialize attributes.

## MF Extra Actions: Add Effects

![image](https://user-images.githubusercontent.com/77353979/163046543-4fa03e29-cffc-4888-945c-da4120161ee7.png)
_Screenshot from [Project Elementus](https://github.com/lucoiso/UEProject_Elementus)_

1. Target Pawn Class: Pawn class that will receive the abilities.
2. Require Tags: Required tags that this actor should own to apply the game framework.
3. Effects Mapping: Gameplay Effects to add and they settings:  
3.1. Effect Class: Gameplay Effect class to add to target.  
3.2. Effect Level: Gameplay Effect Level that will be passed to target ability system component.    
3.3. Set by Caller Params: Set by Caller parameters to pass to this gameplay effect.  
3.3.1. Key (Gameplay Tag): Set By Caller tag.  
3.3.1. Value (Float): Set By Caller float value.  

## MF Extra Actions: Add Enhanced Input Mapping

![image](https://user-images.githubusercontent.com/77353979/163046626-8e5a0295-2770-4dc8-bc2c-6052b826d3d8.png)
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

## MF Extra Actions: Spawn Actors

![image](https://user-images.githubusercontent.com/77353979/163046662-4c19e978-4de8-47f4-8ae6-b422ada74a2b.png)
_Screenshot from [Project Elementus](https://github.com/lucoiso/UEProject_Elementus)_

1. Target Level: Map where this actor will be spawned.
2. Spawn Settings: Array containing actors spawning settings.  
2.1. Actor Class: Actor class to spawn.  
2.2. Spawn transform: Initial transform that will be applied to the spawned actor.  
