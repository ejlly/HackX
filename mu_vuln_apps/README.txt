The principle is easy :

Inside function parse, one notices that a buffer of size 256 is being copied into a buffer of size 128 named "greeting_text"

The function used is strcat, which explicitly warns against this kind of mistake

The fact that the "command" variable is initiated just before the "greeting_text" variable makes is so that a buffer overflow of "greeting_text" will write into "command"

We thus have a way to do arbitrary command execution (as long as it fits in the "command" buffer)

The python script implements the "injection", just write an arbitrary command into the "commands" array in the script to see it injected, and change the value of boolean "showResult" to see a display of the result of the command

Ex:
-Setting 

	commands = ["pwd;whoami"]
	showResult = True

Shows working directory is the root of the disk drive and the fact that we already are root !


Now we find where the website html code is being stored. This is done by using the 2 following commands : 
    commands += ["find / -name *.html 2>&1 test.txt", "cat test.txt"]

First result displayed is : /var/www/html/index.html.
This is the code, and it corresponds to the one shown on the internet browser

<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8" />
<title>INF4743 | μ-challenge</title>
</head>
<body>
<h1>MODAL HACK: μ-challenge</h1>
<hr>
<p>(Write you name in the below to show that you managed to hack this VM)</p>
</body>
</html>

We put the whole website code inside a "website" variable.
It is then looped through to generate all the necessary commands (a single command is too big, so the website code is split into several commands)

Thus, only changing the website variable allows one to put arbitrary website code, very easily (with the website_translator function)

<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8" />
<title>INF4743 | &#956-challenge</title>
</head>
<body>
<h1>MODAL HACK: &#956-challenge</h1>
<hr>
<p>(Write you name in the below to show that you managed to hack this VM)</p>
Charles Nabbout
</body>
</html>

Just one remark, the "mu" greek letter is coded by "&#956" in html, so we changed it in the script

I also added my own name "Charles Nabbout", as required

We then reload the page on the webbrowser, and everything works !
