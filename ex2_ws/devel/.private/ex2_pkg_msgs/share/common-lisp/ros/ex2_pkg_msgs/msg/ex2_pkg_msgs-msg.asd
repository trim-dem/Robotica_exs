
(cl:in-package :asdf)

(defsystem "ex2_pkg_msgs-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "joints" :depends-on ("_package_joints"))
    (:file "_package_joints" :depends-on ("_package"))
    (:file "position" :depends-on ("_package_position"))
    (:file "_package_position" :depends-on ("_package"))
  ))