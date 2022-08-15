As your web application grows bigger it's easy to lose control of its CSS code. This application helps you regain control by going through your code files and showing you the location of style attributes and style tags in HTML code, repeated colors in CSS files, and unused CSS classes.

The application begins by searching for files of the appropriate file type in any given directory. Depending on the feature, it either searches for CSS files or for HTML and PHP files. You can make this search recursive by adding the `-r` option before the directory's path.

## Features & Usage

#### Implemented:

**Show "style" attributes `--style_atts` :** It's bad practice to mix CSS code with HTML code. This option shows you all HTML and PHP files where the "style" attribute is used, along with a list of the elements in which it's used.
```
./csscheck --style_atts [-r] /PATH/TO/HTML/OR/PHP/FILE/OR/FOLDER
```



**Show <style> tags `--style_tags` :** Using style tags isn't as problematic as using style attributes, but you still need at least to keep track of CSS code defined that way. This option shows you all HTML and PHP files in which the style tag is used.
```
./csscheck --style_tags [-r] /PATH/TO/HTML/OR/PHP/FILE/OR/FOLDER
```



#### Work in Progress:

**Show repeated colors `--colors` :** CSS colors which are repeatedly used should be placed inside variables. This feature shows you repeated colors and their occurence frequency. There's support for the keywords, hexadecimal, and RGBA color models.

**Show unused classes `--unused` :** A common problem is that classes defined in CSS files sometimes end up being unused in HTML/PHP files. This feature shows you the paths of CSS files that contain unused classes, along with the names of those classes.

## Build

Currently, you can `clone` the repository and then use `make`.

As time goes on I'll be adding more advanced testing, build, and installation capabilities.

## Compatibility

This application is being developed and tested on Linux, but every effort is being made to make it work on all major UNIX-like systems (i.e. Linux, *BSD, and macOS). Feedback on compatibity is appreciated.

A Windows port is in my long-term plans.

## License

This code is released under the MIT license. To keep it that way, all static libraries used, or will be used, are ones with permissive licenses.

## Notes

_There's a prototype for this application which I wrote in Python and published in [another repository](https://github.com/mutazjustmutaz/cssauditor).

_Don't forget to put paths in double quotes if they contains spaces.

_Feedback is very much appreciated.