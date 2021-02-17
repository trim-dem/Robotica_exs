;; Auto-generated. Do not edit!


(when (boundp 'ex2_pkg_msgs::position)
  (if (not (find-package "EX2_PKG_MSGS"))
    (make-package "EX2_PKG_MSGS"))
  (shadow 'position (find-package "EX2_PKG_MSGS")))
(unless (find-package "EX2_PKG_MSGS::POSITION")
  (make-package "EX2_PKG_MSGS::POSITION"))

(in-package "ROS")
;;//! \htmlinclude position.msg.html


(defclass ex2_pkg_msgs::position
  :super ros::object
  :slots (_position ))

(defmethod ex2_pkg_msgs::position
  (:init
   (&key
    ((:position __position) 0)
    )
   (send-super :init)
   (setq _position (round __position))
   self)
  (:position
   (&optional __position)
   (if __position (setq _position __position)) _position)
  (:serialization-length
   ()
   (+
    ;; uint8 _position
    1
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; uint8 _position
       (write-byte _position s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; uint8 _position
     (setq _position (sys::peek buf ptr- :char)) (incf ptr- 1)
   ;;
   self)
  )

(setf (get ex2_pkg_msgs::position :md5sum-) "6dd1aa2cda989887d566114cab493168")
(setf (get ex2_pkg_msgs::position :datatype-) "ex2_pkg_msgs/position")
(setf (get ex2_pkg_msgs::position :definition-)
      "uint8 position
")



(provide :ex2_pkg_msgs/position "6dd1aa2cda989887d566114cab493168")


