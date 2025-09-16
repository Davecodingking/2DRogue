# 🤖 2D Mech Rogue

A 2D top-down mech roguelike game developed in C++ as a final project for the WM908-15 module. This project leverages the GamesEngineeringBase C++ framework to demonstrate core game development principles and technologies.


<p align="center">
  <a href="https://david-jc.itch.io/2d-spacemecha">
    <img src="https://img.shields.io/badge/Play%20on-itch.io-red?style=for-the-badge&logo=itch.io" alt="Play on itch.io"/>
  </a>
</p>

---

## ✨ Core Features

This game was built to practice and showcase key game engineering technologies. The implementation includes:

* **Dynamic Virtual Camera**: A smooth virtual camera that intelligently follows the player's mech through the game world.
* **Diverse Enemy AI**: A robust NPC system featuring multiple enemy types, each with unique behavioral patterns and attack styles.
* **Precise Collision System**: Accurate collision detection and response between dynamic objects (player, NPCs, projectiles) and the static, tile-based environment.
* **Varied Mech Weaponry**: A flexible attack system allowing the player to switch between linear projectile attacks and powerful Area of Effect (AOE) blasts.
* **Data-Driven Level Design**: Game levels are dynamically constructed by parsing data from external files, allowing for rapid iteration and easy content expansion.
* **Persistent Progress**: A complete game loop supported by a Save/Load system that allows players to save and restore their core progress at any time.

---

## 📸 Gallery

Here are four locations for you to showcase different aspects of your game.

| Player in Action                                                                              | Enemy Variety                                                                                   |
| :-------------------------------------------------------------------------------------------- | :---------------------------------------------------------------------------------------------- |
| ![Player Combat Screenshot](https://placehold.co/400x225/1e1e1e/c8c8c8?text=放置玩家战斗截图)      | ![Enemy Types Screenshot](https://placehold.co/400x225/1e1e1e/c8c8c8?text=放置多种敌人截图)       |
| **(Recommended: A screenshot of the player using an AOE or linear attack)** | **(Recommended: A scene showing at least two different types of enemies)** |
| **Level & Environment** | **UI & Game State** |
| ![Level Design Screenshot](https://placehold.co/400x225/1e1e1e/c8c8c8?text=放置关卡环境截图)      | ![UI Screenshot](https://placehold.co/400x225/1e1e1e/c8c8c8?text=放置游戏UI截图)             |
| **(Recommended: A shot that shows the tile-based world and level structure)** | **(Recommended: A screenshot displaying the score, player health, or other UI elements)** |

---

## 🚀 Getting Started

This project is built upon the **GamesEngineeringBase** C++ framework. To get a local copy up and running, follow these steps.

### Prerequisites

* A C++ compiler (e.g., GCC, Clang, MSVC)
* CMake 3.15 or higher
* The GamesEngineeringBase framework dependency.

### Installation

1.  Clone the repository to your local machine:
    ```sh
    git clone [https://github.com/Davecodingking/2DRogue.git](https://github.com/Davecodingking/2DRogue.git)
    ```
2.  Navigate to the project directory:
    ```sh
    cd 2DRogue
    ```
3.  Configure the project with CMake:
    ```sh
    cmake .
    ```
4.  Build the project. On Windows with Visual Studio, you can open the generated `.sln` file. On Linux or macOS, you can run:
    ```sh
    make
    ```
5.  Run the executable from the build directory.

---

## 🛠️ Technologies Implemented

This project serves as a practical demonstration of the following systems and concepts in C++:

* **Scene Management & Virtual Camera**
* **NPC Behavior and State Machines**
* **Dynamic & Static Collision Detection**
* **Player Attack Mechanisms (Linear & AOE)**
* **File I/O for Data-Driven Design**
* **Core Game Loop & UI Systems**
* **Game State Serialization (Save/Load)**
