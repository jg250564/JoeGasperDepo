<?php
//  bestreads.php
//  Gets the book value, returns the data for
//  the book, title,
//  author, rating, description, and reviews.
//
//  Account: csi345_07
//  File: ~/www/project/bestreads.php
//  Honor Code: We pledge the AIC: Joe Gasper


    // variables for mode and book title
    $mode = $_GET['mode'];
		$book = $_GET['title'];
   
	 	//
		// If mode is info,get info.txt and encode int json array
		//
    if($mode == 'info'){
		   chdir('books/'.$book.'/');
			 $path = "/home/student/csi345_07/www/project/books/".$book."/info.txt";
		   $in = file(strval($path));		 			
			 $instring = array('title' => $in[0],'author' => $in[1], 'stars' => $in[2]);			
			 //print_r($instring);
		   echo json_encode($instring);
		}
		//
		// If mode is description, retreive description.txt
		//
		if($mode == 'description'){
		    chdir('books/'.$book.'/');
		    $path = "/home/student/csi345_07/www/project/books/".$book."/description.txt";
				$desc = file_get_contents($path);			
				//print_r($desc);
		    echo $desc;
		}
		//
		// If mode is reviews, retrive all reviews and place in formatted string
		//
		if($mode == 'reviews'){
		    chdir('books/'.$book.'/');
		    $rev = glob("review*.txt");
				$revList = " ";
				for($i = 0; $i < sizeof($rev);$i++){
				    $path = "/home/student/csi345_07/www/project/books/".$book."/".$rev[$i];
						$tot = file($path);
						$revList .= "<h3>".$tot[0]."</h3><span>".$tot[1]."</span><p>".$tot[2]."</p>";
				}
				//print_r($revList);		
		    echo $revList;
		}
		//
		// If mode is books, retrieve all folders, go through all folders and add
		// folder name as well as title to formatted string.
		//
    if($mode == 'books'){
		   chdir('books');
			 $folders = glob("*");
			 //print_r($folders);
			 $bookList = "<books>";
			 for($i =0; $i<sizeof($folders);$i++){
			      $path = "../books/".$folders[$i]."/info.txt";
			      $title = file($path)[0];
			      $bookList .= "<book><title>".$title."</title><folder>".$folders[$i]."</folder></book>";						
				}
			 $bookList .= "</books>";		                       
       echo $bookList;		 		
		}		
?>