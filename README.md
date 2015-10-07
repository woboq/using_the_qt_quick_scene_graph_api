A QR code animation using the Qt Quick scene graph API
==========

This is the code sample that was used in our Qt World Summit 2015 talk [Using the Qt Quick Scene Graph API](https://youtu.be/cNE6Jabxxxo).

This code runs with Qt 5.5 but the presentation was using two performance patches from Qt 5.6 that were backported locally:

[Avoid recreating QVariantLists when extracted from a QVariant](https://codereview.qt-project.org/123169)

[Avoid rebuiding batches during a material animation](https://codereview.qt-project.org/123166)
