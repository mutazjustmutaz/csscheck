As your web application grows bigger it's easy to lose control of its CSS code. This application helps you regain control by going through your code files and showing you the location of style attributes and style tags in HTML code, repeated colors in CSS files, and unused CSS classes.

The application begins by searching for files of the appropriate file type (either CSS or HTML and PHP) in the directory path[s] you provide. You can make the search recursive by adding the `-r` option before the directory's path.

## Features & Usage

### Implemented:

- **Show "style" attributes `--style_atts` :** It's considered bad practice to mix CSS code with HTML code. This option shows you all HTML and PHP files where the "style" attribute is used, along with a count of the elements in which it's used.
```
./csscheck --style_atts [-r] /PATH/TO/HTML/OR/PHP/FILE/OR/FOLDER
```

![styleatts](https://user-images.githubusercontent.com/74553745/184639945-54fa3bab-17d7-44d0-9aa5-f3fb4bb64c9f.png)


- **Show <style> tags `--style_tags` :** Using style tags isn't as problematic as using style attributes, but you still need at least to keep track of CSS code defined that way. This option shows you all HTML and PHP files in which the "style" tag is used.
```
./csscheck --style_tags [-r] /PATH/TO/HTML/OR/PHP/FILE/OR/FOLDER
```

![styletags](https://user-images.githubusercontent.com/74553745/184640000-faf8249f-be3a-4ad2-8ed7-6227ca12c039.png)

### Work in Progress:

- **Show repeated colors `--colors` :** CSS colors which are repeatedly used should be placed inside variables. This feature shows you CSS files with repeated colors and each color's occurence frequency. There's support for all major color models.

- **Show unused classes `--unused` :** A common problem is that classes defined in CSS files sometimes end up being unused in HTML/PHP files. This feature shows you the paths of CSS files that contain unused classes, along with the names of those classes.

## Build

To build the application, you can use the usual `clone` -> `cd` -> `make` sequence.

As time goes on I'll be adding more advanced testing, build, and installation capabilities.

## Compatibility

This application is being developed and tested on Linux, but every effort is being made to ensure it works on all major UNIX-like systems (i.e. Linux, *BSD, and macOS). Feedback on compatibity is appreciated.

A Windows port is in my long-term plans.

## License

This code is released under the MIT license. To keep it that way, all static libraries used, or will be used, are ones with permissive licenses.

## Notes

_There's a prototype for this application which I wrote in Python and published in [another repository](https://github.com/mutazjustmutaz/cssauditor).

_Don't forget to put paths in double quotes if they contains spaces.

_Feedback is very much appreciated.
