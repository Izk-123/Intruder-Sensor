from django.urls import path
from .views import SensorDataCreate, index

urlpatterns = [
    path('api/sensor-data/',SensorDataCreate.as_view(), name='sensor-data-create'),
    path('', index, name='index')
]