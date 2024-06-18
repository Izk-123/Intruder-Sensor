from django.shortcuts import render
from rest_framework import generics
from .models import SensorData
from .serializers import SensorDataSerializer

class SensorDataCreate(generics.CreateAPIView):
    queryset = SensorData.objects.all()
    serializer_class = SensorDataSerializer

def index(request):
    sensor_data = SensorData.objects.all().order_by('-timestamp')
    template_url = 'mainapp/index.html'
    context = {'sensor_data': sensor_data}

    return render(request, template_url, context)
