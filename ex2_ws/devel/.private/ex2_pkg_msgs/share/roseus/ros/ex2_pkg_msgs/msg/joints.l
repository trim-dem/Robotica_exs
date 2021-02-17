;; Auto-generated. Do not edit!


(when (boundp 'ex2_pkg_msgs::joints)
  (if (not (find-package "EX2_PKG_MSGS"))
    (make-package "EX2_PKG_MSGS"))
  (shadow 'joints (find-package "EX2_PKG_MSGS")))
(unless (find-package "EX2_PKG_MSGS::JOINTS")
  (make-package "EX2_PKG_MSGS::JOINTS"))

(in-package "ROS")
;;//! \htmlinclude joints.msg.html


(defclass ex2_pkg_msgs::joints
  :super ros::object
  :slots (_positions ))

(defmethod ex2_pkg_msgs::joints
  (:init
   (&key
    ((:positions __positions) (let (r) (dotimes (i 0) (push (instance ex2_pkg_msgs::position :init) r)) r))
    )
   (send-super :init)
   (setq _positions __positions)
   self)
  (:positions
   (&rest __positions)
   (if (keywordp (car __positions))
       (send* _positions __positions)
     (progn
       (if __positions (setq _positions (car __positions)))
       _positions)))
  (:serialization-length
   ()
   (+
    ;; ex2_pkg_msgs/position[] _positions
    (apply #'+ (send-all _positions :serialization-length)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; ex2_pkg_msgs/position[] _positions
     (write-long (length _positions) s)
     (dolist (elem _positions)
       (send elem :serialize s)
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; ex2_pkg_msgs/position[] _positions
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _positions (let (r) (dotimes (i n) (push (instance ex2_pkg_msgs::position :init) r)) r))
     (dolist (elem- _positions)
     (send elem- :deserialize buf ptr-) (incf ptr- (send elem- :serialization-length))
     ))
   ;;
   self)
  )

(setf (get ex2_pkg_msgs::joints :md5sum-) "03d6253bad558f8093b6baf0e2461947")
(setf (get ex2_pkg_msgs::joints :datatype-) "ex2_pkg_msgs/joints")
(setf (get ex2_pkg_msgs::joints :definition-)
      "position[] positions
================================================================================
MSG: ex2_pkg_msgs/position
uint8 position
")



(provide :ex2_pkg_msgs/joints "03d6253bad558f8093b6baf0e2461947")


