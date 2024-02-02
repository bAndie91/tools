
# Examples

```
menuprogreload "Recent Files" document-open-recent 3 sh -c 'recent-files-gtk | grep -v ^inode/directory | icewm-menu-recently-used'
menuprogreload "Recent Folders" folder-open-recent 3 sh -c 'recent-files-gtk | icewm-menuhelper-parent-dirs | icewm-menu-recently-used --fullpath-label'
```
