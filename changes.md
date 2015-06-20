## 1.0.1 (2015-06-20)

* Fixes 100% CPU usage (#21)

## 1.0.0 (2015-06-16)

* A lot of changes (see examples and docs)
* Now uses [libqmlbind](https://github.com/seanchas116/libqmlbind)

## 0.0.7 (2014-12-25)

* Support Ruby 2.2 (rc1) and Qt 5.4
* Add `Engine#add_import_path` and `Engine#import_paths` by @tokoro10g

## 0.0.6 (2014-08-19)

* Fix problems when using Fiber with ruby-qml

* Rename block-receiving `QML.application` to `QML.run`

  * `QML.application` without block is still the same name

## 0.0.5 (2014-07-31)

* Support official Qt installation on Mac

## 0.0.4 (2014-07-24)

* Fix 100% CPU usage in idling

## 0.0.3 (2014-07-22)

* Support Ruby 1.9.3

## 0.0.2 (2014-07-21)

* Improve list models

  * Add ArrayModel#replace
  * Add QueryModel for ORMs
  * Specify column names in ListModel#initialize

* Add To-do example with [Sequel](http://sequel.jeremyevans.net/)

* Bug fixes

* Improve README

## 0.0.1 (2014-07-19)

* Initial release
