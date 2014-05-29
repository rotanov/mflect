CONFIG += ordered

TEMPLATE = subdirs

SUBDIRS += tests \
  examples \

examples.depends = tests
