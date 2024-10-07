
## **Build Instructions**
Clone the repository via git clone BattleBots.
cd into the cloned folder, then cd into BattleBots. Run scons platform=[your platform] to build the libraries.
Open up the Godot editor. In the right-side bar, click on the import button.
Click on browse, and navigate to the folder you cloned into. Go BattleBots, Project, and select the project.godot file.
Click on the play button in the top-right corner to play the simulation.

## Description:
**The game is a simple arena battler, where you have to kill enemies before they kill
you! Adapt your strategies for the different enemy types and catch them by
surprise. Collect power ups to boost your survivability.**

**When playing in mutiplayer mode you have to battle your friend untill one of you run out of HP. Dash through them to deal damage and make good use of your flight to avoid them. Pick up HP packs in a pinch and use the terrian to your advantage.** 

# **This is a platformer that was created with the following features:**

## -Player:
 - A player that can move, jump, walk up and down angles, fly, dash, and cling onto walls, and interact with objects. The player is effect by gravity and also effects the speed that it walks up and down the slopes. 
- 3rd person camera control that centers the player around it. 
- Sound effects when moving and picking items up and Background music with background music having separate volume control and on/off control from the sound effects
- Physics and collision detection
- GUI, Status Display, and particle effects.

## -Different AI bots:
The Barbarian: 
- This will try to shoot the player with a laser if it sees them, but runs away if they get too
close.

- This enemy has 3 states:
- **Patrol**. Moves between designated patrol points: 
	- if the player enters the barbarian's vision cone, and the barbarian has line
of sight to the player, it will enter the AimShoot state.
- **AimShoot**. Takes a second to aim at the player, charges up a laser, then shoots.
	- The laser is dark red when its being charged up, but when its bright red it will do
damage.
	- If the player gets too close while the barbarian is aiming, it will transition
to the Run state.
- **Run**. Backs up away from the player.
	- Once the player is no longer too close, transition to the Patrol state.
- How to **defeat** the Barbarian:
	- You can sneak up behind the Barbarian and take it out before it turns around and
sees you.
	- If the Barbarian has already spotted you, if you move to the side as its aiming at
you, you can dodge its laser as it briefly stops tracking you while its charging up
its shot. After dodging its laser, you can dash in close to finish it off.

The Listener
- This enemy is completely blind, instead relying purely on sound and sometimes touch to
identify the player! When it senses the player, it rapidly spins its laser blades.

- Different user actions make different amounts of noise. For example, moving makes a bit
of noise, while landing after a jump makes more noise.
- Attempting to dash or fly when you don’t have the ability to creates a ton of
noise. Don’t spam the dash key!
- The noise you make is relative to distance: the closer you are to the enemy, the
easier it can hear you.
- The enemy has three major states and a few transition states. The major states are:
- **Patrol**, like the Barbarian.
	- Moves between two or more designated patrol points.
- **Seek**. Triggered when the player makes a small amount of noise.
	- Slowly moves towards the location of the noise.
- Turns on its **feel** mode. If the player enters its feel area, it immediately
becomes aggressive and starts spinning. Don’t stand still!
- **Charge**. Triggered when the player makes a large amount of noise.
	- Quickly moves towards the location of the noise.
	- If close enough to the player, it begins to spin, rapidly damaging the player
for small amounts.
	- It can’t really control its movement when spinning, so you can dodge the
spin with good timing!
- You can tell what state the enemy is in through the color of its body. **Green for
patrol, yellow for seek, red for charge.**
- How to **defeat** the Listener:
	- Since the Listener doesn’t have sight it will always keep its defense up when
patrolling, however, when it thinks it hears something and starts to seek that is
when you can strike. You have to be careful to tow the line between getting it to
seek and being loud enough to get it to charge.

**AI Logic**
Implemented two AI types with customizable parameters to allow modularity in agents of the type. I have a finite state machine as the backend of our AI system; both of our AI types use the state machine for the majority of their process logic, save for factors that do not change per state (such as gravity).

The AI types include a state machine, which contains a map of states and is responsible for transitioning between the states. The state machine has a hashmap where the keys are the state names, and the values are references to instances of the states

Implementing the FSM using inheritance and classes makes the system super flexible, and states can be implemented in various ways depending on what the user wants. For example, patrol state is used across both AIs, so in it, I call the agent's get_next_after_patrol() function for the transition logic. So, I can reuse that patrol state across multiple different agents that may have different transition logic. Or, some other states are specific to a certain agent, in which case I can keep all the logic in the state itself to keep things more readable and self-contained.

The barbarian AI uses the same patrol state as the listener, but with different transition conditions. It uses an Area 3D as its vision cone, and uses a raycast to check line of sight with the player. The laser uses another raycast to check if its hitting the player, and to set the appropriate length of the laser so it doesn't look like it's going through objects. The run state uses the NavigationAgent3D to just move in the opposite direction of the player.

## -Network Play: 
- Different Single Player and Multiplayer modes with their own menu options. 
- A lobby system to wait for other players to join and change your name. 
- Different game ending conditions depending if you are playing singleplayer or multiplayer. 
- Ensure different features like player movement, particle effects, object interactions, and timers are communicated and synced between servers. 
- Ability to damage and defeat the other player will all actions updated in real time. 
- A Chatbox to encourage players to interact with each other. When playing against an AI they will use randomly generated taunting phrases.
- If a player disconnects, the other player will win.  


## Controls:
● Space to jump. You are unable to jump in midair.
-      When ledge hanging you can press space to jump off the ledge
● W to move forward, S to move backward, A to move left, D to move right, relative to the
front of the player.
● Q and E to rotate counterclockwise and clockwise, respectively.
● F to toggle flight. When flying, your flight meter depletes. If the flight meter reaches
zero, you are immediately kicked out of flying. When flying:
-     Space to ascend.
-     Shift to descend.
-     Horizontal controls are the same as on the ground.
● Z to grab an item. You can only grab in an area in front of you.
-     Purple items are flight powerups, red items are health power ups.
● Shift to dash (when not in flight mode). You can’t control the player when dashing. Dash
recharges over time.
-     When ledge hanging you can press shift to fall off the ledge
● Mouse Movements control the player camera and allows you to view the world from all
directions, with the camera always focusing on the player.
● Esc to toggle the cursor and interact with menu elements, stops mouse movements from
controlling the camera.

## Power-Ups:

Health  Pack:  
- Pressing  ‘Z’  to  pick  up  the  health  pack  will  restore  your  health  back  to  full.  
- Creates  a  sound  when  picked  up  and  particle  effects.  


Flight  Pack:  
- Pressing  ‘Z’  to  pick  up  the  flight  pack  will  restore  your  flight  meter  back  to  full.  
- Creates  a  sound  when  picked  up  and  particle  effects.

## User Flow:

- At  the  start  screen,  the  user  may  either  choose  to  either  play  singleplayer  or  choose  to  
join  or  create  a  game.  
	- In  singleplayer,  the  end  condition  is  slightly  different:  the  player  has  to  simply  kill  
an  enemy.  The  game  logic  for  single  player  is  very  similar  to  assignment  3.  
- When  the  user  clicks  the  create  game  button:
	- If  a  game  is  not  already  in  place,  the  user  is  taken  to  the  lobby.  The  user  who  
creates  the  game  is  considered  the  server.  
	- If  a  lobby  has  already  been  created,  the  player  is  unable  to  start  a  new  game  -  
there  is  only  one  lobby  active  at  a  time.  
- When  the  user  clicks  the  join  game  button:  
	-	If  a  game  is  not  already  in  place,  the  user  is  taken  to  the  lobby.  
		-If  a  game  is  in  place,  the  user  is  still  taken  to  the  lobby.  However,  instead  of  
signifying  that  they  are  ready,  the  player  can  now  directly  join  the  game.  
- In  the  lobby,  players  may  edit  their  name  and  signify  they  are  ready/start  the  game.  
	-	If  the  other  player  has  not  joined  the  lobby,  the  game  creator  may  immediately  
start  the  game.  
	-	If  the  other  player  has  joined  the  lobby,  the  game  creator  must  wait  until  the  other  
player  signifies  they  are  ready  before  they  can  start  the  game.  
	-	If  the  other  player  disconnects,  the  game  creator  remains  in  the  lobby,  but  if  the  
game  creator  disconnects,  the  other  player  is  sent  back  to  the  start  menu.  
- In  the  game,  if  either  player  dies  or  disconnects,  the  other  player  wins  and  a  game  end  
message  is  shown.  From  here,  the  players  can  quit  back  to  the  title  screen  or  exit.  

## Combat:

 In  Singleplayer:  You  fight  the  listener  bot:  
	-	How  to  defeat  the  Listener:  
- Since  the  Listener  doesn’t  have  sight  it  will  always  keep  its  defense  up  
when  patrolling,  however,  when  it  thinks  it  hears  something  and  starts  to  
seek  that  is  when  you  can  strike.  You  have  to  be  careful  to  tow  the  line  
between  getting  it  to  seek  and  being  loud  enough  to  get  it  to  charge.  
- In  Multiplayer:  Your  objective  is  to  defeat  the  other  player.  
	-	You  can  accomplish  this  by  dashing  into  them  to  chunk  down  their  HP,  once  it  
reaches  0  you  have  won.  
	-	It  takes  2  hits  to  defeat  the  player  without  power  ups  and  4  if  they  get  the  health  
power  up.

## Credit:
● The skybox texture was found royalty-free on Pixabay by myersalex216.
● The “boing” and “blade” sound effects were also found royalty-free on Pixabay.
● The “error” sound and “robot_death” sound was found royalty-free on mixkit.co.
● Many  of  the  taunts  are  references  to  other  media,  including:  
- The  video  game  ULTRAKILL  
- The  video  game  Street  Fighter  V 
- The  TV  series  Adventure  Time  
- The  anime  Jojo’s  Bizarre  Adventure

**Everything that is not listed listed was created by me.** 

Demo Video: https://youtu.be/OMqSuXT_BI4 <- Lobby / Multiplayer Showcase

            https://youtu.be/NAEKMaM5eYY < - AI Showcase


*Shows single & recent Commit due to migration from Gitlab -> Github
