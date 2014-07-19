# ruby-qml

[![Gem Version](https://badge.fury.io/rb/qml.svg)](http://badge.fury.io/rb/qml)
[![Build Status](https://travis-ci.org/seanchas116/ruby-qml.svg?branch=master)](https://travis-ci.org/seanchas116/ruby-qml)
[![Coverage Status](https://coveralls.io/repos/seanchas116/ruby-qml/badge.png?branch=master)](https://coveralls.io/r/seanchas116/ruby-qml?branch=master)

ruby-qml is a QML / Qt Quick wrapper for Ruby.
It provides bindings between QML and Ruby and enables you to use Qt Quick-based GUI from Ruby.

* [Documentation](http://rubydoc.info/github/seanchas116/ruby-qml/master/frames)
* [Examples](https://github.com/seanchas116/ruby-qml/tree/master/examples)

## Installation

ruby-qml currently requires **Ruby 2.0 or later**.

### OS X with Homebrew

Run the following commands to install ruby-qml on OS X with Homebrew:

    $ brew install pkg-config
    $ brew install libffi
    $ brew install qt5
    $ gem install qml -- --with-libffi-dir=$(brew --prefix libffi) --with-qt-dir=$(brew --prefix qt5)

### General

#### Requirements

* pkg-config
* libffi
* Qt 5.2 or later

To install, use this command after installing requirements:

    $ gem install qml

### Use Gemfile

Add this line to your Gemfile:

    gem 'qml'

And then execute:

    $ bundle install

To pass build options, use `bundle config`.
For example:

    $ bundle config build.qml --with-libffi-dir=$(brew --prefix libffi) --with-qt-dir=$(brew --prefix qt5)

The configuration will be saved in `~/.bundle/config`

## Gallery

[![Screenshot](https://raw.github.com/seanchas116/ruby-qml/master/examples/fizzbuzz/capture.png)](https://github.com/seanchas116/ruby-qml/tree/master/examples/todo)
[![Screenshot](https://raw.github.com/seanchas116/ruby-qml/master/examples/twitter/capture.png)](https://github.com/seanchas116/ruby-qml/tree/master/examples/twitter)

## Usage

### Load QML file

The following code loads a QML file and shows an application window titled "Hello, world!".

```ruby
QML.application do |app|
  app.load_path Pathname(__FILE__) + '../main.qml'
end
```

```qml
// main.qml
import QtQuick 2.2
import QtQuick.Controls 1.1

ApplicationWindow {
    visible: true
    width: 200
    height: 100
    title: "Hello, world!"
}
```

### Use Ruby class in QML

To make your class available to QML, include `QML::Access` and call `register_to_qml`.

By including `QML::Access`, you can also define **properties and signals** in Ruby classes like in QML.

Properties are used to bind data between QML and Ruby.
Signals are used to provide the observer pattern-like notification from Ruby to QML.

![Screenshot](https://raw.github.com/seanchas116/ruby-qml/master/examples/fizzbuzz/capture.png)

```ruby
# Ruby
class FizzBuzz
  include QML::Access
  register_to_qml under: "Example", version: "1.0"

  property :input, '0'
  property :result , ''
  signal :inputWasFizzBuzz, []

  on_changed :input do
    i = input.to_i
    self.result = case
    when i % 15 == 0
      inputWasFizzBuzz.emit
      "FizzBuzz"
    when i % 3 == 0
      "Fizz"
    when i % 5 == 0
      "Buzz"
    else
      i.to_s
    end
  end

  def quit
    puts "quitting..."
    QML.application.quit
  end
end
```

```qml
// QML - main.qml
import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import Example 1.0

ApplicationWindow {
    visible: true
    width: 200
    height: 200
    title: "FizzBuzz"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        TextField {
            placeholderText: "Input"
            text: "0"
            id: textField
        }
        Text {
            y: 100
            id: text
            text: fizzBuzz.result
        }
        Button {
            text: 'Quit'
            onClicked: fizzBuzz.quit()
        }
        Text {
            id: lastFizzBuzz
        }
    }
    FizzBuzz {
        id: fizzBuzz
        input: textField.text
        onInputWasFizzBuzz: lastFizzBuzz.text = "Last FizzBuzz: " + textField.text
    }
}
```

You can omit arguments of `register_to_qml` if they are obvious:

```ruby
module Example
  VERSION = '1.0.0'

  class FizzBuzz
    include QML::Access
    register_to_qml

    ...
  end
end
```

If the Ruby object is singleton, you can use the root context to make it available to QML.
In this case, you don't have to use `register_to_qml`.

```ruby
class Foo
  include QML::Access
  def foo
    puts "foo"
  end
end

QML.application do |app|
  app.context[:foo] = Foo.new
  app.load_path Pathname(__FILE__) + '../main.qml'
end
```

### Pass data to QML ListModels

To bind list data between QML ListView and Ruby, you can use ListModels.

* `QML::Data::ListModel` - the base class for ruby-qml list models.

* `QML::Data::ArrayModel` - provides a simple list model implementation using Array.

This example uses `ArrayModel` to provide list data for a QML ListView.
When the content of the ArrayModel is changed, the list view is also automatically updated.

```ruby
# Ruby
class TodoModel < QML::Data::ArrayModel
  column :title, :description, :due_date
end

class TodoController
  include QML::Access
  register_to_qml under: "Example", version: "1.0"

  property :model, TodoModel.new

  def add(title, description, due_date)
    item = OpenStruct.new(
      title: title,
      description: description,
      due_date: due_date)
    p item
    model << item
  end
end
```

```qml
// QML
ListView {
    model: todo.model
    delegate: Text {
        text: "Title: " + title + ",  Description: " + description + ", Due date: " + due_date
    }
}
TodoController {
  id: todo
}
```

### Combile asynchronous operations

In QML, all UI-related operations are done synchronously in the event loop.
To set result of asynchronous operations to the UI, use `QML.later` or `QML::Dispatchable#later`.

```ruby
# Ruby
class HeavyTaskController
  # QML::Access includes QML::Dispathable
  include QML::Access
  register_to_qml under: "Example", version: "1.0"

  property :result, ''

  def set_result(result)
    self.result = result
  end

  def start_heavy_task
    Thread.new do
      self.later.set_result do_heavy_task() # #set_result is called in the main thread in the next event loop
      # or
      QML.later do
        set_result do_heavy_task()
      end
    end
  end
end
```

```qml
// QML
Text {
  text: controller.result
}
Button {
  text: "Start!!"
  onClicked: controller.start_heavy_task()
}
HeavyTaskController {
  id: controller
}
```

### Use Qt objects in Ruby

In ruby-qml, Qt objects (QObject-derived C++ objects and QML objects) can be accessed from Ruby via the meta-object system of Qt.

You can access:

* Properties
* Signals
* Slots (as methods), Q_INVOKAVLE methods, QML methods

You cannot access:

* Normal C++ member functions

If their names are camelCase in Qt, ruby-qml aliases them as underscore_case.

```ruby
# QML::Application is a wrapper for QApplication
app = QML.application

# set property
app.applicationName = "Test"
app.application_name = "Test" # aliased version

# connect to signal
app.aboutToQuit.connect do # "about_to_quit" is also OK
  puts "quitting..."
end

# call method (slot)
app.quit
```

### Value conversions

The following types are automatically converted between Ruby and QML:

* Integer
* Double
* String
* Time
* Date
* DateTime
* Array
* Hash
* QML::Geometry::Point (QPoint, QPointF)
* QML::Geometry::Size (QSize, QSizeF)
* QML::Geometry::Rectangle (QRect, QRectF)
* QML::QtObjectBase (Qt objects)
* QML::Access
* QML::Data::ListModel

### Load and use Qt C++ plugins

`PluginLoader` loads Qt C++ plugins.
It enables you to use your Qt C++ codes from Ruby easily.


```c++
// C++ - plugin example
class MyPlugin : public QObject
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.myplugin.MyPlugin")
public slots:
    void foo() {
        qDebug() << "foo";
    }
};
```

```ruby
# Ruby
plugin = QML::PluginLoader.new(directory, "myplugin").instance
plugin.foo
```

### Garbage collection

To support garbage collection of Qt objects used in ruby-qml,
`#managed?` attribute of each Qt object wrappr determines its memory management status.

#### Managed objects

*Manged objects* are managed by Ruby and QML and garbage collected when no longer reachable.
All objects created inside QML and objects returned from C++ methods will be *managed* by default.

#### Unmanaged objects

*Unmanaged* objects are not managed and never garbage collected.
Objects that have parents or that obtained from properties of other Qt objects will be *unmanaged* by default.

#### Specify management status explicitly

The `#managed?` method returns whether the object is managed or not.
The `#prefer_managed` methods sets management status safely
(e.g., objects that are created by QML will remain managed and objects that have parents will remain unmanaged).

```ruby
plugin = PluginLoader.new(path).instance
obj = plugin.create_object
obj.prefer_managed false
```

## Contributing

Contributions are welcome. When you are contributing to ruby-qml:

1. Fork it ( http://github.com/seanchas116/ruby-qml/fork )
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
