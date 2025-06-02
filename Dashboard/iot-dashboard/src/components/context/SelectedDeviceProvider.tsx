'use client'

import { getDeviceListInfo, getTelemetryTb } from '@/actions'
import { createContext, useState, ReactNode, useEffect, useMemo } from 'react'

export const SelectedDeviceContext = createContext({
    deviceList: [],
    deviceWithCoords: [],
    geojson: { type: 'FeatureCollection', features: [] },
    selectedDevice: null,
    refreshKey: 0,
    setSelectedDevice: () => { },
    refreshDeviceList: () => { }
})

export function SelectedDeviceProvider({ children }: { children: ReactNode }) {
    const [selectedDevice, setSelectedDevice] = useState(null)
    const [deviceList, setDeviceList] = useState([])
    const [deviceWithCoordsList, setDeviceWithCoordsList] = useState([])
    const [refreshKey, setRefreshKey] = useState<number>(0)

    useEffect(() => {
        async function fetchDevices() {
            try {
                const token = localStorage.getItem('token') ?? ''
                const res = await getDeviceListInfo(10, 0, token)
                setDeviceList(res)
            } catch (err) {
                console.error('Failed to load devices', err)
            }
        }
        fetchDevices()
    }, [refreshKey])

    const refreshDeviceList = () => setRefreshKey((k) => k + 1)

    useEffect(() => {
        if (deviceList.length === 0) return

        let mounted = true
        async function enrichWithCoords() {
            const token = localStorage.getItem('token') ?? ''
            const enriched = []

            // Promise.all để gọi song song
            const enrichDevices = await Promise.all(
                deviceList.map(async (dev) => {
                    try {
                        const value = await getTelemetryTb(dev.id.id, token, ['lat', 'long'])
                        return {
                            ...dev,
                            latitude: parseFloat(value.lat[0].value ? value.lat[0].value : 0),
                            longitude: parseFloat(value.long[0].value ? value.long[0].value : 0),
                        }
                    } catch {
                        // nếu lỗi, có thể bỏ qua hoặc gán mặc định
                        return null
                    }
                })
            )

            if (mounted) {
                setDeviceWithCoordsList(enrichDevices)
            }
        }

        enrichWithCoords()
        return () => {
            mounted = false
        }
    }, [deviceList])

    const geojson = useMemo(() => {
        return {
            type: 'FeatureCollection',
            features: deviceWithCoordsList.map((dev) => {

                return {
                    type: 'Feature',
                    id: dev.id.id,
                    geometry: {
                        type: 'Point',
                        coordinates: [dev.longitude, dev.latitude],
                    },
                    properties: {
                        title: dev.name ? dev.name : 'N/A',
                        description: `Lat: ${dev.latitude}, Lng: ${dev.longitude}`,
                    },
                }
            }),
        }
    }, [deviceWithCoordsList])

    return (
        <SelectedDeviceContext.Provider value={{ deviceList, geojson, selectedDevice, refreshKey, setSelectedDevice, refreshDeviceList }}>
            {children}
        </SelectedDeviceContext.Provider>
    )
}