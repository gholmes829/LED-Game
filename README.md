# LED-Game
Program for LED Game run on Arduino

Three LEDs and three buttons are connected to Arduino. Lights flash n times while user tries to memorize sequence.
 *              User then has to press buttons corresponding to LEDs in correct order. EG if LEDs flash red->green->blue, user presses red, green, then blue button.
 *              If they remembered the sequence and inputted it back in correctly, they advance to the next level with increased difficulty.
 *              Has three levels of difficulty that user selects on device with buttons.
 *              Also has secret "insane" mode with random and sporadic delays to increase difficulty by holding all three buttons down at startup.
 *              Sequences are randomly generated each time.
