Launch PokerStarsStreamHelper.exe. It will start as a console application, just minimize it.

It will create all the text files required

it does however need a 'grabber-config.ini' file, there is an example one. you can adjust settings in there for table positions and number of stages etc

replay stage table and tables with duplicate tournaments IDs are excluded from the table list and table count

If you close a stage table, it won't update until you move another table onto it, so at the end of a stream if you're 1 tabling and want to 
update the Main Stage list, just close and restart the PokerStarsTitleGrabber.exe and it will clear all the stages txt files.

If you open a table that doesn't produce the title you would expect then record and let me know and I can fix it up in the next version

# Chat_Stage_Break_Message.txt

'Tables currently on break. Action will resume in 3:54' 

countdown bettwen X:55 and X+1:00

and between X:00 and X:02 will have

'The action will resume momentarily...'

# Homegames - currently supported names:

The Palace Open
Slumdog Championship

