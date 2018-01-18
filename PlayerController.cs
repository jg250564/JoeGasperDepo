using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour {

	public float moveSpeed;            // variable for player speed
	private float moveVelocity;        // variable for player movement
	public float jumpHeight;           // variable for player jump height

	public Transform groundCheck;        // transform object for detecting ground
	public float groundCheckRadius;      // size of groundCheck
	public LayerMask whatisGround;       // layer for what is ground
	private bool grounded;               // tells if on ground

	private bool doubleJumped;          // tells if doubleJumping

	private Animator anim;             // animator object

	public Transform upperFirePoint;        // transform object for detecting where to shoot from
	public Transform lowerFirePoint;        // transform object for detecting where to shoot from
	public Transform firePoint;        // transform object for detecting where to shoot from
	public GameObject ninjaStar;       // ninjaStar object
    public GameObject upBullet;       // upBullet object
    public GameObject downBullet;       // downBullet object
    public GameObject machineGunBullet;       // machineGunBullet object
	//public GameObject diagonalBullet;       // diagonalBullet object
	public GameObject deathRay;         // deathRay object

	public float shotDelay;             // float for delay between shots
	private float shotDelayCounter;     // counter for shots

    public float knockback;             // determines knockback
    public float knockbackLength;       // determines length of knockBack
    public float knockbackCount;        // counter for knockBacks
    public bool knockfromRight;         // determines if being knockedBackfromRight
    public bool knockedBack;            // tells if being knockedBack

	private Rigidbody2D myrigidbody2D;       // Player's Rigidbody2D

	public bool rapidFire;        // tells if in rapid fire

    public bool onLadder;         // Variables to climb ladder JG 3/24/17
    public float climbSpeed;
    private float climbVelocity;
    private float gravityStore;

    public bool speedMode;      // variable to tell if speedMode is active 
    public float speedUp;        // variable to set speedMode speed

	

	/// <summary>
	/// Audio stuff
	/// </summary>
	public AudioClip clipShot;
	public AudioClip clipJetPack;
	private AudioSource audioShoot;
	private AudioSource audioJump;


	public AudioSource AddAudio(AudioClip clip, bool loop, bool playAwake, float vol) { 
		AudioSource newAudio = gameObject.AddComponent<AudioSource>();
		newAudio.clip = clip; 
		newAudio.loop = loop;
		newAudio.playOnAwake = playAwake;
		newAudio.volume = vol; 
		return newAudio; 
	}

	public void Awake(){
		// add the necessary AudioSources:
		audioShoot = AddAudio(clipShot, false, false, 0.7F);
		audioJump = AddAudio(clipJetPack, false, false, 1F);
	}


	// Use this for initialization
	void Start () {
		anim = GetComponent<Animator>();           // set anim to Animator for player

		myrigidbody2D = GetComponent<Rigidbody2D> ();   // set myrigidbody2D to player Rigidbody2D

		knockbackCount = 0;							// Emily added this so that the character doesn't knockback at the load of the scene.
		grounded = true;
		doubleJumped = false;

		rapidFire = false;
        speedMode = false;
        gravityStore = myrigidbody2D.gravityScale;        // Stores gravity for climbing ladders JG 3/24/17
//		audioShoot = GetComponent<AudioSource>();	// AUDIO STUFF
//		audioJump = GetComponent<AudioSource>();	// AUDIO STUFF
	}

	// Establishes if grounded
	void FixedUpdate() {

		grounded = Physics2D.OverlapCircle (groundCheck.position, groundCheckRadius, whatisGround);
	}
	// Update is called once per frame
	void Update () {
		
//		if (grounded) {                           // if grounded...
//			doubleJumped = false;	             // cannot doubleJump
//		}
		if (grounded) {                            // if grounded...
			doubleJumped = false;	             // cannot doubleJump
			audioJump.Stop();
//			audioJump.Play();
		} 

//		if (Input.GetKeyUp (KeyCode.UpArrow)) {
//			audioJump.Stop ();
//		}


		anim.SetBool ("Grounded", grounded);         // set anim for grounded
		anim.SetBool ("DoubleJump", doubleJumped);   // set anim for doubleJumped
		anim.SetBool("knockedBack", knockedBack);   // set anim for knockedBack

        /*if (doubleJumped) {                   // debugging script
			Debug.Log ("Double Jump");
		}
		else {
			Debug.Log ("NO - Double Jump");
		}*/


       

        if (Input.GetKeyDown (KeyCode.UpArrow) && grounded ){                  //if up arrow is pressed and player is grounded...
			

			Jump ();                                                           // perform jump
		}

		//if ((Input.GetKeyDown (KeyCode.UpArrow) && !doubleJumped && !grounded) || (Input.GetKeyDown (KeyCode.Space) && !doubleJumped && !grounded)){

		if (Input.GetKeyDown (KeyCode.UpArrow) && !doubleJumped && !grounded){     // if up is pressed and player is not doubleJumped or grounded...		

			Jump ();               // perform jump(doubleJump)
			doubleJumped = true;       // player is doubleJumping
		}




		moveVelocity = 0f;             // moveVelocity set 

        if (speedMode == true)
        {

            if ((Input.GetKey(KeyCode.RightArrow)) || (Input.GetKey(KeyCode.D)))
            {


                moveVelocity = speedUp;   // moveVelocity is set to speedUp, resulting in moving right ( by increasing moveVelocity)
            }

            // if left arrow or 'a' is pressed...

            if ((Input.GetKey(KeyCode.LeftArrow)) || (Input.GetKey(KeyCode.A)))
            {


                moveVelocity = -speedUp;  // moveVelocity is set to moveSpeed, resulting in moving left ( by decreasing moveVelocity)
            }
        }

        else
        {
            // Code to move left and right
            // if right arrow or 'd' is pressed...

            if ((Input.GetKey(KeyCode.RightArrow)) || (Input.GetKey(KeyCode.D))) {


                moveVelocity = moveSpeed;   // moveVelocity is set to moveSpeed, resulting in moving right ( by increasing moveVelocity)
            }

            // if left arrow or 'a' is pressed...

            if ((Input.GetKey(KeyCode.LeftArrow)) || (Input.GetKey(KeyCode.A))) {


                moveVelocity = -moveSpeed;  // moveVelocity is set to moveSpeed, resulting in moving left ( by decreasing moveVelocity)
            }
        }
        

        // if knockback is 0 or less...

        if (knockbackCount <= 0)
        {
//			Debug.Log ("We're not going to knockback. The knockback count: " + knockbackCount);
			myrigidbody2D.velocity = new Vector2(moveVelocity, myrigidbody2D.velocity.y);      //adjust y velocity
            knockedBack = false;
            // if knockback is greater than 0...
        }
        else
        {
//			Debug.Log ("We're going to knockback. The knockback count: " + knockbackCount);
            knockedBack = true;

            if (knockfromRight)
				myrigidbody2D.velocity = new Vector2(- knockback,knockback);          // if knocked from right, push player left and vertically
            if(!knockfromRight)
				myrigidbody2D.velocity = new Vector2(knockback, knockback);          // if knocked from left, push player right and vertically
           
            knockbackCount -= Time.deltaTime;                                        // reduce knockbackCount
        }

		anim.SetFloat ("Speed", Mathf.Abs (myrigidbody2D.velocity.x));                     // Set anim speed


		// if player's velocity is greater than 0...

		if (myrigidbody2D.velocity.x > 0)
			transform.localScale = new Vector3 (1f, 1f, 1f);       // move player right
		// if player's velocity is less than 0...
		else if (myrigidbody2D.velocity.x < 0)
			transform.localScale = new Vector3 (-1f, 1f, 1f);     // move player left

		// if right alt, left alt, return, or space are pressed... (single fire)
		if (rapidFire == false) {
			
			if (Input.GetKeyDown (KeyCode.Space)) {
				Instantiate (ninjaStar, firePoint.position, firePoint.rotation);                 // make ninjaStar and fire
				shotDelayCounter = shotDelay;                                                 // adjust shotDelayCounter
				Instantiate (ninjaStar, firePoint.position, firePoint.rotation);        // make ninjaStar and fire
				audioShoot.Play ();
			}
			if(Input.GetKeyDown (KeyCode.RightAlt) || Input.GetKeyDown (KeyCode.LeftAlt)){
				Instantiate (upBullet, upperFirePoint.position, upperFirePoint.rotation);                 // make ninjaStar and fire
				shotDelayCounter = shotDelay;                                                 // adjust shotDelayCounter
				Instantiate (upBullet, upperFirePoint.position, upperFirePoint.rotation);        // make ninjaStar and fire
				audioShoot.Play ();
			}
			if(Input.GetKeyDown (KeyCode.RightControl) || Input.GetKeyDown (KeyCode.LeftControl) || Input.GetKeyDown(KeyCode.RightCommand) || Input.GetKeyDown(KeyCode.RightControl)){
					Instantiate (downBullet, lowerFirePoint.position, lowerFirePoint.rotation);                 // make ninjaStar and fire
					shotDelayCounter = shotDelay;                                                 // adjust shotDelayCounter
					Instantiate (downBullet, lowerFirePoint.position, lowerFirePoint.rotation);        // make ninjaStar and fire
					audioShoot.Play ();
			}
			// For Diagonal Bullets
			//if (Input.GetKeyDown (KeyCode.Space) && Input.GetKey(KeyCode.UpArrow)) {
				//Instantiate (diagonalBullet, firePoint.position, firePoint.rotation);                 // make ninjaStar and fire
				//shotDelayCounter = shotDelay;                                                 // adjust shotDelayCounter
				//Instantiate (ninjaStar, firePoint.position, firePoint.rotation);        // make ninjaStar and fire
				//audioShoot.Play ();
			//}
		}else if(rapidFire == true){
				// if right alt, left alt, return, or space are pressed... (auto fire)
				if ((Input.GetKey (KeyCode.RightAlt)) || (Input.GetKey (KeyCode.LeftAlt)) || (Input.GetKey (KeyCode.Return)) || (Input.GetKey (KeyCode.Space))) {
			
					shotDelayCounter -= Time.deltaTime;                                          // adjust shotDelayCounter

					// if shotDelayCounter is less than 0...
					if (shotDelayCounter <= 0) {
						shotDelayCounter = shotDelay;                                           // adjust shotDelayCounter
					Instantiate (machineGunBullet, firePoint.position, firePoint.rotation);        // make ninjaStar and fire
						audioShoot.Play ();
					}
				}
        
	     }
            
	
        // Code for climbing ladders JG 3/24/17
        if (onLadder)                                 // if onLadder...
        {
            myrigidbody2D.gravityScale = 0f;      // No Gravity

            climbVelocity = climbSpeed * Input.GetAxisRaw("Vertical");

            myrigidbody2D.velocity = new Vector2(myrigidbody2D.velocity.x, climbVelocity);
                 
        }
        if (!onLadder)                          // if exiting ladder...
        {
            myrigidbody2D.gravityScale = gravityStore;   // restore gravity
        }

	}

	// makes player jump
	public void Jump()
	{
		myrigidbody2D.velocity = new Vector2 (myrigidbody2D.velocity.x, jumpHeight);    // change player's y to +jumpHeight
		audioJump.Play();
	}


	// Makes player stay still on a moving platform...

	// or if destroy terrian
	void OnCollisionEnter2D(Collision2D other)
	{
		//Debug.Log ("The player is colliding with: " + other.transform.tag);
		if(other.transform.tag == "MovingPlatform")    // if player enters a tagged "MovingPlatform"
		{
			transform.parent = other.transform;         // player becomes child of platform
		}

		if (other.transform.tag == "CrumbleBox") 
		{
			other.transform.GetComponent<DestroyObjectOverTime> ().startDestroy = true;
		}
	}
	void OnCollisionExit2D(Collision2D other)
	{
		if (other.transform.tag == "MovingPlatform")   // if player exits a tagged "MovingPlatform"
		{
			transform.parent = null;          // player is no longer child of platform
		}
	}






}