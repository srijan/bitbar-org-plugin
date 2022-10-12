## Fork notes

* Made compatible with Linux
* Tested with [Kargos](https://github.com/lipido/kargos)

*Original README below*

## Org Mode TODO Plugin

[Bitbar](https://github.com/matryer/bitbar) plugin to display the number of open Org mode TODO items. This entire program could easily be replaced by a simple shell script, but where's the fun in that...

![screenshot](screenshot.png)

### Installation

No external dependencies are required. Configuration options can be found in `config.h`. The plugin can be built using a simple invocation of make.

```sh
$ make
```

To enable, link to bitbar's plugin directory with an added useless suffix since bitbar is unable to match the refresh interval portion of a plugin filename if it isn't formatted exactly as three period separated fields.

```sh
$ ln -s $PWD/plugin <plugin_dir>/plugin.2h.x
```
