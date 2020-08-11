#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
; #Warn  ; Enable warnings to assist with detecting common errors.
SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.
SetWorkingDir %A_ScriptDir%  ; Ensures a consistent starting directory.

;^Numpad1::MoveWindow(LocX, LocY, SizeX, SizeY)

^+q::MoveWindow(-7, 0, 600, 400)
^+w::MoveWindow(635, 0, 600, 400)
^+e::MoveWindow(1275, 0, 600, 400)
^+a::MoveWindow(-7, 480, 600, 400)
^+s::MoveWindow(635, 480, 600, 400)
^+d::MoveWindow(1275, 480, 600, 400)
^+f::MoveWindow(1275, 490, 600, 400)

; ### DO NOT EDIT BELOW ###

MoveWindow(LocX, LocY, SizeX, SizeY)
{
WinGetActiveTitle, TitleVar
WinMove, %TitleVar%,, LocX, LocY, SizeX, SizeY
;Send, {F5}
}