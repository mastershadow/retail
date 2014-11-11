retail
======
Reversed following tail *nix tool.
Useful if you want to have an unbuffered source reversed on your screen.

For example:  
`tail -f access.log | retail`

If `tail -f` gives you: 
```
1   
2 
3
4
... and after a while...
N
```

With `tail -f | retail` you get
```
4
3
2
1
```
and then when `N` comes in
```
N
4
3
...
```

That's it. *Following tail with reversed order*.

Retail is a nightly project by Eduard Roccatello.  
Made with love for Guly and his tremendous peppers.  
Greets to #openbsd on AzzurraNet ;)

By Eduard 'Master^Shadow' Roccatello (http://www.roccatello.com)  
My loving company __3DGIS__ (http://www.3dgis.it)
