>When searching through the git commits, we notice that the "my_secrets.py", which is now in the .gitignore, was at some point committed to the git page.

We thus gain access through login :

username : superXWebDeveloper
password : bA8wd5NkGvtMdaYgBbD3

>Then, we run ssh-keygen on the client machine

>We then run the script hack.py, which allows our public key to be written automatically to /home/web/.ssh/authorized_keys

This is done by first exploiting the fact that if we know at which time we will press the "Validate" button on the jackpot page, we know the seed, and thus the outcome before it has even been drawn.

The script implements that automatically.

Then, we change the "date" parameter of the winner page to "../.ssh/authorized_keys" to write on the right file, and the "name" parameter to the contents of "id_rsa.pub", our public key.

It is automatically injected.

>We then ssh connect via the following command :

ssh -i ~/.ssh/id_rsa web@5.48.81.223

>Once in, we notice that we haven't got the rights to access the required files.

Yet, a "sudo -l" command reveals that user "superxwebdeveloper" is allowed to use vim without a password.

Thus, using "sudo -u superxwebdeveloper vim" allows us to launch vim with super-user privileges.

One can then read the required files (or even execute all apps with elevated privileges, by using the vim execution commands (":!ls" for example) )
