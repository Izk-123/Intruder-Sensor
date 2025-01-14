from django.db import models

# Create your models here.
class SensorData(models.Model):
    distance = models.FloatField()
    timestamp = models.DateTimeField(auto_now_add=True)

    def __str__(self):
        return f"{self.distance} cm at {self.timestamp}"
