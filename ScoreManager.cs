using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class ScoreManager : MonoBehaviour {

	public static int score;

	Text text;

	void Start()
	{
		text = GetComponent<Text> ();

		//score = 0;
		score = PlayerPrefs.GetInt("CurrentPlayerScore");


	}

	void Update()
	{
		if (score < 0)
			score = 0;

		text.text = "" + score;

	}

	public static void AddPoints(int pointsToAdd)
	{

		score += pointsToAdd;
        PlayerPrefs.SetInt("CurrentPlayerScore", score);
	}

	void OnDestroy() {
		PlayerPrefs.SetInt ("CurrentPlayerScore", score);
	}
		

	public static void Reset()
	{
		score = 0;
        PlayerPrefs.SetInt("CurrentPlayerScore", score);

    }

	void OnApplicationQuit() {
		Reset ();
	}
}
