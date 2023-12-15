example fstab entry:

execfuse#/src/execfuse/autosshfs  /mnt/ssh/!conn  fuse  auto_unmount,default_permissions,allow_root

option allow_root is neccessary to make mount.sshfs happy.
