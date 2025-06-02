"use client"

import 'mapbox-gl/dist/mapbox-gl.css';
import mapboxgl from 'mapbox-gl';
import { useContext, useEffect, useRef } from 'react'
import { SelectedDeviceContext } from './context/SelectedDeviceProvider';
import { getTelemetryTb } from '@/actions';

const test = {
    type: 'FeatureCollection',
    features: [
        {
            id: 'c1',
            type: 'Feature',
            geometry: {
                type: 'Point',
                coordinates: [106.66087502218016, 10.773505694766392]
            },
            properties: {
                title: 'Mapbox',
                description: 'A3 Ly Thuong Kiet'
            }
        },
        {
            id: 'c2',
            type: 'Feature',
            geometry: {
                type: 'Point',
                coordinates: [106.65823775282203, 10.769162638008975]
            },
            properties: {
                title: 'Mapbox',
                description: 'San Francisco, California'
            }
        }
    ]
};

export const MapInterface = () => {
    const { geojson, selectedDevice } = useContext(SelectedDeviceContext)
    // const mapContainerRef = useRef('');
    const mapRef = useRef<mapboxgl.Map>()
    const containerRef = useRef<HTMLDivElement>(null)
    const markersRef = useRef<
        Array<{ id: string; marker: mapboxgl.Marker }>
    >([])
    const wsMapRef = useRef()

    useEffect(() => {
        mapboxgl.accessToken = process.env.NEXT_PUBLIC_MAPBOX_API_ACCESS_TOKEN;

        const map = new mapboxgl.Map({
            container: containerRef.current!,
            center: [106.657, 10.77], // starting position [lng, lat]
            zoom: 14 // starting zoom
        });

        mapRef.current = map

        return () => {
            map.remove()
            markersRef.current = []
        }

    }, []);

    useEffect(() => {
        console.log('geojson >>>', geojson)
        geojson.features.forEach((feature) => {
            const popup = new mapboxgl.Popup({ offset: 25, className: 'custom-popup' }).setHTML(`
                    <div>
                        <strong class="text-base font-bold mb-4 inline-block">${feature.properties.title}</strong><br/>
                        Loading telemetry…
                    </div>
                `)
            //     .setHTML(`
            //         <div>
            //         <div>Sensor C1</div>
            //         <div><strong>Temperature</strong>: 20.8oC</div>
            //         <div><strong>Humidity</strong>: 49 %</div>
            //         <a href="/">Sensor details</a>
            //         </div>
            // `)

            popup.on('open', async () => {
                const c = popup.getElement()?.querySelector('.mapboxgl-popup-content')
                c?.classList.add('popup-open')

                if (wsMapRef.current && wsMapRef.current[feature.id]) return

                const token = localStorage.getItem('token') ?? ''

                const ws = new WebSocket(process.env.NEXT_PUBLIC_TB_WS_URL!)
                if (wsMapRef.current) wsMapRef.current[feature.id] = ws

                ws.onopen = () => {
                    ws.send(JSON.stringify({
                        authCmd: { cmdId: 0, token },
                        cmds: [{
                            entityType: 'DEVICE',
                            entityId: feature.id,   // dùng đúng ID của device
                            scope: 'LATEST_TELEMETRY',
                            cmdId: 10,
                            type: 'TIMESERIES',
                        }]
                    }))
                }

                ws.onmessage = (evt) => {
                    const msg = JSON.parse(evt.data)
                    const d = msg.data || {}
                    const pH = parseFloat(d.pH?.[0]?.[1])?.toFixed(0) ?? 'N/A'
                    const ORP = parseFloat(d.ORP?.[0]?.[1])?.toFixed(0) ?? 'N/A'
                    const TUR = parseFloat(d.TUR?.[0]?.[1])?.toFixed(0) ?? 'N/A'
                    const EC = parseFloat(d.EC?.[0]?.[1])?.toFixed(0) ?? 'N/A'

                    popup.setHTML(`
                      <div>
                        <strong class="text-base font-bold mb-4 inline-block">${feature.properties.title}</strong><br/>
                        <div class="mb-2"><strong class="font-semibold underline">pH</strong>: <span class="font-bold text-red-500">${pH}</span></div>
                        <div class="mb-2"><strong class="font-semibold underline">ORP</strong>: <span class="font-bold text-red-500">${ORP}</span> mV</div>
                        <div class="mb-2"><strong class="font-semibold underline">TUR</strong>: <span class="font-bold text-red-500">${TUR}</span> NTU</div>
                        <div class="mb-5"><strong class="font-semibold underline">EC</strong>: <span class="font-bold text-red-500">${EC}</span> μS/cm</div>
                        <a href="/dashboard/${feature.id}" class="text-blue-600 hover:underline text-sm">Sensor details</a>
                      </div>
                    `)
                }

                ws.onclose = () => {
                    delete wsMapRef.current?.[feature.id]
                }
            })
            popup.on('close', () => {
                const c = popup.getElement()?.querySelector('.mapboxgl-popup-content')
                c?.classList.remove('popup-open')

                const ws = wsMapRef.current?.[feature.id]
                if (ws) {
                    ws.close()
                    delete wsMapRef.current?.[feature.id]
                }
            })

            const marker = new mapboxgl.Marker({ color: 'black' })
                .setLngLat(feature.geometry.coordinates as [number, number])
                .setPopup(popup)
                .addTo(mapRef.current!)
            markersRef.current.push({ id: feature.id!, marker })
        })
    }, [geojson])

    useEffect(() => {
        if (!selectedDevice || !mapRef.current) return

        const token = JSON.parse(JSON.stringify(localStorage.getItem('token')))


        const getDevicePosition = async () => {
            const value = await getTelemetryTb(selectedDevice.id.id, token, ['lat', 'long'])

            mapRef.current && mapRef.current.flyTo({
                center: [value.long[0].value, value.lat[0].value,],
                zoom: 16,
                essential: true,
                speed: 0.8,
                curve: 1.8,
                easing: (t) => t
            })

            markersRef.current.forEach(({ id: fid, marker }) => {
                const el = marker.getElement() as HTMLElement
                if (fid === selectedDevice.id.id) {
                    // highlight: phóng to, đổi màu
                    el.style.transform = 'scale(1.5)'
                    el.style.backgroundColor = 'red'
                    marker.togglePopup() // mở popup
                } else {
                    // reset marker khác
                    el.style.transform = ''
                    el.style.backgroundColor = ''
                    if (marker.getPopup().isOpen()) marker.togglePopup()
                }
            })
        }
        getDevicePosition()

    }, [selectedDevice])

    return <div
        style={{
            height: '100%'
        }}
        ref={containerRef}
        className="map-container"
    />
}